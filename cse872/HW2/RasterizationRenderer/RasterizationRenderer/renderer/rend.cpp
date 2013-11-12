#include "stdafx.h"
#include "Gz.h"
#include <math.h>
#include <vector>
using namespace std;

int GzRender::PutCamera(GzCamera	*camera)
{
   /*
   - overwrite renderer camera structure with new camera definition
   */
   this->camera.FOV = camera->FOV;
   vectorCopy(this->camera.lookat, camera->lookat);
   vectorCopy(this->camera.position, camera->position);
   vectorCopy(this->camera.worldup, camera->worldup);
   return GZ_SUCCESS;
}

int GzRender::PushMatrix(GzMatrix	matrix)
{
   /*
   - push a matrix onto the stack
   - check for stack overflow
   */
   if(matlevel>=MATLEVELS-1)
      return GZ_FAILURE;

   matrixMult(Ximage[matlevel+1], Ximage[matlevel], matrix);

   // For the transformation of normal, only the 3x3 rotation block matters
   GzMatrix rotation;
   loadIdentity(rotation);
   if (matlevel >= 2) { 
      //ignore the perspective projection and viewport transformation.
      rotation[0][0] = matrix[0][0];	rotation[0][1] = matrix[0][1];	rotation[0][2] = matrix[0][2];
      rotation[1][0] = matrix[1][0];	rotation[1][1] = matrix[1][1];	rotation[1][2] = matrix[1][2];
      rotation[2][0] = matrix[2][0];	rotation[2][1] = matrix[2][1];	rotation[2][2] = matrix[2][2];
   }
   matrixMult(Xnorm[matlevel+1], Xnorm[matlevel], rotation);

   matlevel++;
   return GZ_SUCCESS;
}


int GzRender::PopMatrix()
{
   /*
   - pop a matrix off the stack
   - check for stack underflow
   */
   if (matlevel == 0)
      return GZ_FAILURE;

   matlevel--;
   return GZ_SUCCESS;
}

GzRender::GzRender(GzRenderClass	r_class,GzDisplay	*display)
{
   /* 
   - keep closed until all inits are done
   - setup Xsp and anything only done once
   - init default camera
   */
   this->display = display;
   this->open = false;
   this->r_class = r_class;
   loadIdentity(Xsp);
   Xsp[0][0] = Xsp[0][3] = display->xres/2.f;
   Xsp[1][1] = display->yres/2.f; Xsp[1][3] = display->yres/2.f;
   Xsp[2][2] = (float) INT_MAX;

   matlevel = 0;
   loadIdentity(Ximage[0]);
   loadIdentity(Xnorm[0]);

   numlights = 0;

   camera.FOV = DEFAULT_FOV;
   GzCoord lookat = {0., 0., 0.};
   GzCoord position = {DEFAULT_IM_X, DEFAULT_IM_Y, DEFAULT_IM_Z};
   GzCoord worldup = {0., 1., 0.};

   vectorCopy(camera.lookat, lookat);
   vectorCopy(camera.position, position);
   vectorCopy(camera.worldup, worldup);
}


GzRender::~GzRender()
{
   /* 
   -free all renderer resources
   */
   this->open = false;
}


int GzRender::Begin()
{
   /* 
   - set up for start of each frame - clear frame buffer
   - compute transformation matrices.
   */
   display->Init();
   GzCoord u, v, n, r;

   vectorCopy(r, camera.position);
   vectorSubtract(n, camera.lookat, camera.position);
   normalize(n);

   GzCoord t;
   scaling(t, dot(camera.worldup, n), n);
   vectorSubtract(v, camera.worldup, t);
   normalize(v);

   vectorCross(u, v, n);

   GzMatrix & Xiw = camera.Xiw;

   loadIdentity(camera.Xiw);
   Xiw[0][0] = u[0]; Xiw[0][1] = u[1]; Xiw[0][2] = u[2]; Xiw[0][3] = - dot(r, u);
   Xiw[1][0] = v[0]; Xiw[1][1] = v[1]; Xiw[1][2] = v[2]; Xiw[1][3] = - dot(r, v);
   Xiw[2][0] = n[0]; Xiw[2][1] = n[1]; Xiw[2][2] = n[2]; Xiw[2][3] = - dot(r, n);

   loadIdentity(camera.Xpi);

   float d_inverse = tan(acos(-1.f)/180.f*camera.FOV/2.f);

   camera.Xpi[2][2] = camera.Xpi[3][2] = d_inverse;

   PushMatrix(Xsp);
   PushMatrix(camera.Xpi);
   PushMatrix(camera.Xiw);
   return GZ_SUCCESS;
}


int GzRender::PutAttribute( int numAttributes, GzToken* nameList, GzPointer* valueList )
{
   /*
   - set renderer attribute states (e.g.: uniform variables, shaders, texture maps, and lights)
   */
   for (int i = 0; i < numAttributes; i++) {
      switch (nameList[i]) {
      case GZ_VERTEX_SHADER:
         vertex_shader = (GzShader) valueList[i];
         break;
      case GZ_FRAGMENT_SHADER:
         fragment_shader = (GzShader) valueList[i];
         break;
      case GZ_DIRECTIONAL_LIGHT:
         vectorCopy(lights[numlights].color, ((GzLight*) (valueList[i]))->color);
         vectorCopy(lights[numlights].direction, ((GzLight*) (valueList[i]))->direction);
         numlights++;
         break;
      case GZ_AMBIENT_LIGHT:
         vectorCopy(ambientlight.color, ((GzLight*) (valueList[i]))->color);
         break;
      case GZ_AMBIENT_COEFFICIENT:
         vectorCopy(Ka,  *((GzColor*) (valueList[i])));
         break;
      case GZ_DIFFUSE_COEFFICIENT:
         vectorCopy(Kd, *((GzColor*) (valueList[i])));
         break;
      case GZ_SPECULAR_COEFFICIENT:
         vectorCopy(Ks, *((GzColor*) (valueList[i])));
         break;
      case GZ_DISTRIBUTION_COEFFICIENT:
         spec = *((float*) (valueList[i]));
         break;
      case GZ_TEXTURE_MAP:
         tex_fun = (GzTexture) (valueList[i]);
         break;
      default:
         break;
      }
   }

   return GZ_SUCCESS;
}

void GzRender::Rasterize(int num, GzToken * typeList, GzPointer * list[3])
{
   // num is the number of output variables in the vertex shader

   GzCoord * vertexList[3];
   GzPointer * fragment_input_list;

   fragment_input_list = new GzPointer[num];

   //Assume vertex position is the first output of vertex shader
   vertexList[0] = (GzCoord*) (list[0][0]);
   vertexList[1] = (GzCoord*) (list[1][0]);
   vertexList[2] = (GzCoord*) (list[2][0]);

   //And allocate memory for the input list of the fragment shader
   allocate_list(num, typeList, fragment_input_list);

   // compute the bounding box
   float xmin, ymin, xmax, ymax;
   int x_start, y_start, x_end, y_end;

   bounding_box(vertexList, xmin, ymin, xmax, ymax, x_start, y_start, x_end, y_end);
   int xDim = x_end - x_start +1;

   double e[3], a[3], b[3];
   lineEquations(e, a, b, vertexList, x_start, y_start);

   GzToken * outputType;
   GzPointer * outList;
   int numOutput;
   for (int y = y_start; y <= y_end; y++)
   {
      for (int x = x_start; x<= x_end; x++)
      {
         if (e[0] > 0 && e[1] > 0 && e[2] > 0) {
            assemble_fragment_input(num, typeList, fragment_input_list, list, e);
            fragment_shader(num, typeList, fragment_input_list, numOutput, outputType, outList, this);
            GzColor *color = (GzColor*) (outList[0]); // assuming color is the only output
            int z = (int) (*(GzCoord*) (fragment_input_list[0]))[Z];
            display->Put(x,y, ctoi((*color)[RED]),ctoi((*color)[GREEN]), ctoi((*color)[BLUE]), 1, z);
            clean_up_list(numOutput, outputType, outList);
            delete [] outputType;
         }
         e[0] += a[0];
         e[1] += a[1];
         e[2] += a[2];
      }
      e[0] += -xDim* a[0] + b[0];
      e[1] += -xDim* a[1] + b[1];
      e[2] += -xDim* a[2] + b[2];
   }

   //clean up memory
   clean_up_list(num, typeList, fragment_input_list);
}


int GzRender::DrawTriangles(int numTriangles, int numParts, /* how many names and values */ GzToken *nameList, GzPointer *valueList)
{
   /* 
   - pass in a triangle description with tokens and values corresponding to
   GZ_NULL_TOKEN:		do nothing - no values
   GZ_POSITION:		3 vert positions in model space
   - Invoke the scan converter and return an error code
   */
   GzPointer *inList[3];
   inList[0] = new GzPointer[numParts];
   inList[1] = new GzPointer[numParts];
   inList[2] = new GzPointer[numParts];
   int numOutput;
   GzToken * outputType;
   GzPointer * outList[3];

   for (int i=0; i < numTriangles; i++) 
   {
      //all input variables for vertices 0,1,2 of the vertex are put in inList[0], inList[1], and inList[2], resp.
      assemble_vertex_input(numParts, nameList, inList, valueList, i);
      //Vertex shaders can run in parallel, we only process the three we need for a single triangle
      vertex_shader(numParts, nameList, inList[0], numOutput, outputType, outList[0], this);
      delete[] outputType;
      vertex_shader(numParts, nameList, inList[1], numOutput, outputType, outList[1], this);
      delete[] outputType;
      vertex_shader(numParts, nameList, inList[2], numOutput, outputType, outList[2], this);

      Rasterize(numOutput, outputType, outList);

      clean_up_list(numOutput, outputType, outList[0]);
      clean_up_list(numOutput, outputType, outList[1]);
      clean_up_list(numOutput, outputType, outList[2]);

      delete[] outputType;
   }

   delete[] inList[0];
   delete[] inList[1];
   delete[] inList[2];

   return GZ_SUCCESS;
}


