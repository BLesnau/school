#include "stdafx.h"
#include	"Gz.h"
#include  <limits.h>

GzDisplay::GzDisplay(GzDisplayClass disp_class,int xRes,int yRes)
{
   /* create a display:
   -- allocate memory for indicated class and resolution
   -- pass back pointer to GzDisplay object in display
   */
   this->xres = xRes;
   this->yres = yRes;
   this->disp_class = disp_class;
   this->open = false;
   this->fbuf = new GzPixel[xRes*yRes];
   this->image = new BYTE[xRes*yRes*3];
}


GzDisplay::~GzDisplay()
{
   /* clean up, free memory */
   delete[] fbuf;
   delete[] image;
}


int GzDisplay::GetParams(int	*xRes, int	*yRes, GzDisplayClass	*disp_class)
{
   /* pass back values for an open display */
   *xRes = this->xres;
   *yRes = this->yres;
   *disp_class = this->disp_class;

   return GZ_SUCCESS;
}


int GzDisplay::Init()
{
   /* set everything to some default values - start a new frame */
   int size = xres * yres;
   for (int i = 0; i < size; i++) {
      fbuf[i].alpha = (1<<12)-1;
      fbuf[i].red = 0x60<<4;
      fbuf[i].green = 0x70<<4;
      fbuf[i].blue = 0x60<<4;
      fbuf[i].z = INT_MAX;
   }

   open = true;
   return GZ_SUCCESS;
}

inline int index(const int&i, const int &j, const int &res)
{
   return res*j+i;
}

int GzDisplay::Put(int i, int	j, GzIntensity	r, GzIntensity	g, GzIntensity	b, GzIntensity	a, GzDepth z)
{
   /* write pixel values into the display */
   if( i<0 || i >=xres || j < 0 || j >= yres )
   {
      return GZ_FAILURE;
   }

   GzPixel & pixel = fbuf[index(i,j,xres)];

   if( pixel.z >= z )
   {
      pixel.alpha = a;
      pixel.red = r;
      pixel.green = g;
      pixel.blue = b;
      pixel.z = z;
   }

   return GZ_SUCCESS;
}


int GzDisplay::Get(int	i,int	j,GzIntensity	*r,GzIntensity	*g,GzIntensity	*b,GzIntensity	*a,GzDepth		*z)
{
   /* pass back pixel value in the display */
   /* check display class to see what vars are valid */
   if (i<0 || i >=xres || j < 0 || j >= yres)
      return GZ_FAILURE;

   GzPixel & pixel = fbuf[index(i,j,yres)];
   *a = pixel.alpha;
   *r = pixel.red;
   *g = pixel.green;
   *b = pixel.blue;
   *z = pixel.z;

   return GZ_SUCCESS;
}


/* Convert an intensity value to a 8-bit intensity value */
static char ByteIntensity(GzIntensity intensity)
{
   intensity >>= 4;
   return (char)((intensity > 255) ? 255 : intensity);
}

int GzDisplay::Flush()
{
   if (!open)
      return GZ_FAILURE;

   /* write pixels */

   int size = xres * yres;
   for (int i = 0; i < size; i++) {
      GzPixel & pixel = fbuf[i];
      image[3*i+2] = ByteIntensity(pixel.red);
      image[3*i+1] = ByteIntensity(pixel.green);
      image[3*i+0] = ByteIntensity(pixel.blue);
   }
   return GZ_SUCCESS;
}




