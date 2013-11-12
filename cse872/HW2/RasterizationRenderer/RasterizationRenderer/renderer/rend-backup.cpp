#include "stdafx.h"
#include "Gz.h"
#include <math.h>
#include <vector>
using namespace std;
#include "linearAlgebra.h"

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
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	if(matlevel>=MATLEVELS-1)
		return GZ_FAILURE;

	matrixMult(Ximage[matlevel+1], Ximage[matlevel], matrix);
	matlevel++;
	return GZ_SUCCESS;
}


int GzRender::PopMatrix()
{
/*
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if (matlevel == 0)
		return GZ_FAILURE;
	
	matlevel--;
	return GZ_SUCCESS;
}

///////
void loadIdentity(GzMatrix m)
{
	m[0][0] = 1.; m[0][1] = 0.; m[0][2] = 0.; m[0][3] = 0.;
	m[1][0] = 0.; m[1][1] = 1.; m[1][2] = 0.; m[1][3] = 0.;
	m[2][0] = 0.; m[2][1] = 0.; m[2][2] = 1.; m[2][3] = 0.;
	m[3][0] = 0.; m[3][1] = 0.; m[3][2] = 0.; m[3][3] = 1.;
}

GzRender::GzRender(GzRenderClass	r_class,GzDisplay	*display)
{
/* 
- malloc a renderer struct
- keep closed until all inits are done
- setup Xsp and anything only done once
- span interpolator needs pointer to display
- check for legal class GZ_Z_BUFFER_RENDER
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
- compute Xiw and projection xform Xpi from camera definition
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw - now
	stack contains Xsw and app can push model Xforms if it want to.
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


int GzRender::PutAttribute(int		numAttributes,GzToken	*nameList,GzPointer	*valueList)
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
	for (int i = 0; i < numAttributes; i++) {
		switch (nameList[i]) {
			case GZ_RGB_COLOR:
				{
					float * rcolor = flatcolor.data();
					float * color = (float *) (valueList[i]);
					rcolor[0] = color[0]; rcolor[1] = color [1]; rcolor[2] = color[2];
				}
				break;
			case GZ_VERTEX_SHADER:
				vertex_shader = (GzShader) valueList[i];
				break;
			case GZ_FRAGMENT_SHADER:
				fragment_shader = (GzShader) valueList[i];
				break;
			default:
				break;
		}
	}

	return GZ_SUCCESS;
}

void increment(GzCoord *edge, GzCoord *delta)
{
   (*edge)[X] += (*delta)[X];
   (*edge)[Z] += (*delta)[Z];
}

void differenceX(GzCoord *v1, GzCoord *v2, GzCoord *s, GzCoord *ds, int x1)
{
	float f = x1 - (*v1)[X];
	float d_inverse = 1.f/((*v2)[X]-(*v1)[X]);
	(*ds)[Z] = ((*v2)[Z] - (*v1)[Z]) *d_inverse ;
	(*s)[Z]   = (*v1)[Z] +  f * (*ds)[Z];
}
void differenceY(GzCoord *v1, GzCoord *v2, GzCoord *s, GzCoord *ds, int y1)
{
	float f = y1 - (*v1)[Y];
	float d_inverse = 1.f/((*v2)[Y]-(*v1)[Y]);

	(*ds)[X] = ((*v2)[X] - (*v1)[X]) *d_inverse ;
	(*s)[X]   = (*v1)[X] +  f * (*ds)[X];

	(*ds)[Z] = ((*v2)[Z] - (*v1)[Z]) *d_inverse ;
	(*s)[Z]   = (*v1)[Z] +  f * (*ds)[Z];
}

short	ctoi(float color);		/* convert float color to GzIntensity short */
void GzRender::scanX(GzCoord *l, GzCoord *r, int y)
{
	int x, lx, rx;
	GzCoord s, ds;

	lx = (int) ceil((*l)[X]);
	rx = (int) ceil((*r)[X]);

 	int no_backward_face_culling = 1;
	if(lx <= rx) {
      differenceX(l, r, &s, &ds, lx);
			for(x = lx; x < rx; x++) {
				display->Put(x,y, ctoi(flatcolor[RED]),ctoi(flatcolor[GREEN]), ctoi(flatcolor[BLUE]), 1, int(s[Z]));
        increment(&s,&ds);
			}
	} else if (no_backward_face_culling) {
      differenceX(r, l, &s, &ds, rx);
			for(x = rx; x < lx; x++) {
				display->Put(x,y, ctoi(flatcolor[RED]),ctoi(flatcolor[GREEN]), ctoi(flatcolor[BLUE]), 1, int(s[Z]));
        increment(&s,&ds);
			}
	}
}

int GzRender::scanY(GzCoord**	vertexList, int i, int num, GzToken * typeList, GzPointer * list[3])
{
   int li, ri; 	// left & right upper endpoint indices
   int ly, ry;	// left & right upper endpoint y values
   GzCoord l, dl;	// current left edge and delta
   GzCoord r, dr;	// current right edge and delta
   int rem;	// number of remaining vertices
   int y;		// current scanline

   li = ri = i;
   ly = ry = y = (int) ceil((*vertexList[i])[Y]);

	 rem = 3;
	 while( rem > 0) {
      // find appropriate left edge
		 while( ly <= y && rem > 0) {
			 rem--;
			 i = li - 1; if(i < 0) i = 2;  // go clockwise
			 ly = (int) ceil( (*vertexList[i])[Y] );
			 if( ly > y ) // replace left edge
					differenceY( &(*vertexList[li]), &(*vertexList[i]), &l, &dl, y);
			 li = i;
		 }
      // find appropriate right edge
		 while( ry <= y && rem > 0) {
			 rem--;
			 i = ri + 1; if(i > 2) i = 0;  // go counter-clockwise
			 ry = (int) ceil( (*vertexList[i])[Y]);
			 if( ry > y ) // replace right edge
					differenceY( &(*vertexList[ri]), &(*vertexList[i]), &r, &dr, y);
			 ri = i;
		 }
      // while l & r span y (the current scanline)
         // draw the span
		 for( ; y < ly && y < ry; y++) {
			 // scan and interpolate edges
			 scanX(&l, &r, y);
			 increment(&l,&dl);
			 increment(&r,&dr);
		 }

	 }

	 return GZ_SUCCESS;
}

void lineEquations(float *e, float* a, float* b,GzCoord** vertexList, int x_start, int y_start)
{
	//line equations for all three edges of the triangle: e(x,y) = a x + by + c 
	b[0] = (*(vertexList[1]))[X] - (*(vertexList[0]))[X];
	a[0] = (*(vertexList[0]))[Y] - (*(vertexList[1]))[Y];
	e[0] = a[0] * (x_start-(*(vertexList[0]))[X]) + b[0] * (y_start-(*(vertexList[0]))[Y]);

	b[1] = (*(vertexList[2]))[X] - (*(vertexList[1]))[X];
	a[1] = (*(vertexList[1]))[Y] - (*(vertexList[2]))[Y];
	e[1] = a[1] * (x_start-(*(vertexList[1]))[X]) + b[1] * (y_start-(*(vertexList[1]))[Y]);


	b[2] = (*(vertexList[0]))[X] - (*(vertexList[2]))[X];
	a[2] = (*(vertexList[2]))[Y] - (*(vertexList[0]))[Y];
	e[2] = a[2] * (x_start-(*(vertexList[2]))[X]) + b[2] * (y_start-(*(vertexList[2]))[Y]);
}

void GzRender::Rasterize(int num, GzToken * typeList, GzPointer * list[3])
{
	// num is the number of output variables in the vertex shader

	GzCoord * vertexList[3];

	//Extract the vertex list from the output list of the vertex shader
	for (int j = 0; j < num; j++) {
		switch (typeList[j]) {
		case GZ_POSITION:
			vertexList[0] = (GzCoord*) (list[0][j]);
			vertexList[1] = (GzCoord*) (list[1][j]);
			vertexList[2] = (GzCoord*) (list[2][j]);
			break;
		default:
			break;
		}
	}

	// compute the bounding box
	float xmin, ymin, xmax, ymax;
	xmax = xmin = (*vertexList[0])[X];
	ymax = ymin = (*vertexList[0])[Y];
	for (int i = 1; i < 3; i++) {
		if ( (*vertexList[i])[X] < xmin ) {
			xmin = (*vertexList[i])[X];
		}
		if ( (*vertexList[i])[Y] < ymin) {
			ymin = (*vertexList[i])[Y];
		}
		if ( (*vertexList[i])[X] > xmax) {
			xmax = (*vertexList[i])[X];
		}
		if ( (*vertexList[i])[Y] > ymax) {
			ymax = (*vertexList[i])[Y];
		}
	}

	int x_start, y_start, x_end, y_end;
	x_start = (int) ceil(xmin);
	y_start = (int) ceil(ymin);
	x_end = (int) floor(xmax);
	y_end = (int) floor(ymax);
	int xDim = (int) x_end - x_start + 1;

	float e[3], a[3], b[3];
	lineEquations(e, a, b, vertexList, x_start, y_start);

	for (int y = y_start; y <= y_end; y++)
	{
		for (int x = x_start; x<= x_end; x++)
		{
			if (e[0] > 0 && e[1] > 0 && e[2] >0)
				display->Put(x,y, ctoi(flatcolor[RED]),ctoi(flatcolor[GREEN]), ctoi(flatcolor[BLUE]), 1, int((*vertexList[0])[Z]));
			if (e[0] < 0 && e[1] < 0 && e[2] <0)
				display->Put(x,y, ctoi(flatcolor[RED]),ctoi(flatcolor[GREEN]), ctoi(flatcolor[BLUE]), 1, int((*vertexList[0])[Z]));
			e[0] += a[0];
			e[1] += a[1];
			e[2] += a[2];
		}
		e[0] += -xDim* a[0] + b[0];
		e[1] += -xDim* a[1] + b[1];
		e[2] += -xDim* a[2] + b[2];
	}
	//int imin = 0;

	//for (int i = 0; i < 3; i++) {
	//	if ( (*vertexList[i])[Y] <= (*vertexList[imin])[Y]) {
	//		imin = i;
	//	}
	//}
	//
	//scanY(vertexList, imin, num, typeList, list);
}


int GzRender::DrawTriangles(int numTriangles, int	numParts,	/* how many names and values */ GzToken	*nameList, GzPointer	*valueList)
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

	for (int i=0; i < numTriangles; i++) {
		for (int j = 0; j < numParts; j++) {
			switch (nameList[j]) {
			case GZ_NULL_TOKEN:
				break;
			case GZ_POSITION:
				inList[0][j] = (GzPointer*) &(*(vector<GzCoord> *) (valueList[j]))[3*i];
				inList[1][j] = (GzPointer*) &(*(vector<GzCoord> *) (valueList[j]))[3*i+1];
				inList[2][j] = (GzPointer*) &(*(vector<GzCoord> *) (valueList[j]))[3*i+2];
				break;
			case GZ_NORMAL:
				inList[0][j] = (GzPointer*) &(*(vector<GzCoord> *) valueList[j])[3*i];
				inList[1][j] = (GzPointer*) &(*(vector<GzCoord> *) valueList[j])[3*i+1];
				inList[2][j] = (GzPointer*) &(*(vector<GzCoord> *) valueList[j])[3*i+2];
				break;
			default:
				break;
			}
		}
		//Vertex shaders can run in parallel, we only process the three we need for a single triangle
		vertex_shader(numParts, nameList, inList[0], numOutput, outputType, outList[0], this);
		delete[] outputType;
		vertex_shader(numParts, nameList, inList[1], numOutput, outputType, outList[1], this);
		delete[] outputType;
		vertex_shader(numParts, nameList, inList[2], numOutput, outputType, outList[2], this);
		
		Rasterize(numOutput, outputType, outList);

		for (int j = 0; j < numOutput; j++) {
			switch (outputType[j]) {
			case GZ_NULL_TOKEN:
				break;
			case GZ_POSITION:
			case GZ_NORMAL:
				delete (GzCoord*) (outList[0][j]);
				delete (GzCoord*) (outList[1][j]);
				delete (GzCoord*) (outList[2][j]);
				break;
			default:
				break;
			}

		}
		delete[] outputType;
		delete[] outList[0];
		delete[] outList[1];
		delete[] outList[2];
	}

	delete[] inList[0];
	delete[] inList[1];
	delete[] inList[2];

	return GZ_SUCCESS;
}



/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}




