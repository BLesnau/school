/* Texture functions for GzLib	*/
#include	"stdafx.h"
#include	"stdio.h"
#include	"Gz.h"

int	reset = 1;
FILE	*fd;
int	xs, ys;
GzColor image[1024*1024];

void InitTex()
{
   char  		foo[8];
   int   		i;
   unsigned char pixel[3];

   if (reset) {          /* open and load texture file */
      fopen_s(&fd, "data\\texture", "rb");
      if (fd == NULL) {
         TRACE("texture file not found\n");
         return;
      }
      fseek(fd, 0L, SEEK_SET);
      fscanf_s(fd, "%s %d %d %s\n", foo, 8,  &xs, &ys, foo, 8);
      if (xs*ys > 1024*1024)
         TRACE("texture too large\n");
      for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
         fread(pixel, sizeof(pixel), 1, fd);
         image[i][RED] = (float)((int)pixel[RED]) * (1.0f / 255.0f);
         image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0f / 255.0f);
         image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0f / 255.0f);
      }
      reset = 0;          /* init is done */
   }
}

GzColor BilinearInterpolateColors( float x, float y, GzColor cTL, GzColor cTR, GzColor cBL, GzColor cBR )
{
   float xDiff = x - (int)x;
   float xDiffInv = 1 - xDiff;
   GzColor topColor = { cTL[RED]*xDiffInv + cTR[RED]*xDiff,
      cTL[GREEN]*xDiffInv + cTR[GREEN]*xDiff,
      cTL[BLUE]*xDiffInv + cTR[BLUE]*xDiff };
   GzColor bottomColor = { cBL[RED]*xDiffInv + cBR[RED]*xDiff,
      cBL[GREEN]*xDiffInv + cBR[GREEN]*xDiff,
      cBL[BLUE]*xDiffInv + cBR[BLUE]*xDiff };

   float yDiff = y - (int)y;
   float yDiffInv = 1 - yDiff;
   GzColor endColor = { topColor[RED]*yDiffInv + bottomColor[RED]*yDiff,
      topColor[GREEN]*yDiffInv + bottomColor[GREEN]*yDiff,
      topColor[BLUE]*yDiffInv + bottomColor[BLUE]*yDiff };

   return endColor;
}

GzColor GetTexPixel( int x, int y )
{
   InitTex();

   return image[x + y * xs];
}

/* Image texture function */

int tex_fun(float u,float v, GzColor& color)
{
   InitTex();

   /* bounds-test u,v to make sure nothing will overflow image array bounds */
   /* determine texture cell corner values and perform bilinear interpolation */
   /* set color to interpolated GzColor value and return */
   if (u < 0.)
      u = 0.;
   if (v < 0.)
      v = 0.;
   if (u > 1.)
      u = 1.;
   if (v > 1.)
      v = 1.;

   float x = u*xs;
   float y = v*ys;

   GzPoint TL = {(int)x, (int)y};
   GzPoint TR = {(int)x+1, (int)y};
   GzPoint BL = {(int)x, (int)y+1};
   GzPoint BR = {(int)x+1, (int)y+1};

   if( u >= 1 )
   {
      TR[0] -= 1;
      BR[0] -= 1;
   }

   if( v >= 1 )
   {
      BL[1] -= 1;
      BR[1] -= 1;
   }

   GzColor cTL = GetTexPixel( TL[0], TL[1] );
   GzColor cTR = GetTexPixel( TR[0], TR[1] );
   GzColor cBL = GetTexPixel( BL[0], BL[1] );
   GzColor cBR = GetTexPixel( BR[0], BR[1] );

   color = BilinearInterpolateColors( x, y, cTL, cTR, cBL, cBR );

   return 0;
}

GzColor GetPTexPixel( int x, int y )
{
   GzColor color;

   if ( (((int) floor(x*16.f))+ (int) floor(y*16.f)) % 2 == 0 ) {
      color[0] = 1.f;
      color[1] = 0.f;
      color[2] = 0.f;
   } else {
      color[0] = 0.f;
      color[1] = 0.f;
      color[2] = 1.f;
   }

   return color;
}

/* Procedural texture function */
int ptex_fun_crossed(float u,float v,GzColor& color)
{
   if ( (((int) floor(u*16.f))) % 2 == 0 || (((int) floor(v*16.f))) % 2 == 0 ) {
      color[0] = 1.f;
      color[1] = 0.f;
      color[2] = 0.f;
   } else {
      color[0] = 0.f;
      color[1] = 0.f;
      color[2] = 1.f;
   }

   return 0;
}

int ptex_fun_horiz(float u,float v,GzColor& color)
{
   if ( ((int) floor(v*16.f)) % 2 == 0 ) {
      color[0] = 1.f;
      color[1] = 0.f;
      color[2] = 0.f;
   } else {
      color[0] = 0.f;
      color[1] = 0.f;
      color[2] = 1.f;
   }

   return 0;
}

int ptex_fun_vert(float u,float v,GzColor& color)
{
   if ( ((int) floor(u*16.f)) % 2 == 0 ) {
      color[0] = 1.f;
      color[1] = 0.f;
      color[2] = 0.f;
   } else {
      color[0] = 0.f;
      color[1] = 0.f;
      color[2] = 1.f;
   }

   return 0;
}