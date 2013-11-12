/* Texture functions for GzLib	*/
#include	"stdafx.h"
#include	"stdio.h"
#include	"Gz.h"


/* Image texture function */
GzColor image[1024*1024];
int tex_fun(float u,float v, GzColor& color)
{
  static int	reset = 1;
  static FILE	*fd;
  static int	xs, ys;
  char  		foo[8];
  int   		i, j;
  unsigned char pixel[3];

  if (reset) {          /* open and load texture file */
    fopen_s(&fd, "data\\texture", "rb");
    if (fd == NULL) {
      TRACE("texture file not found\n");
      return(-1);
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

	//remove this line after you  implemented the proper interpolation
	color[RED] = 0; 	color[GREEN] = 0;	color[BLUE] = 0; 

  return 0;
}


/* Procedural texture function */

int ptex_fun(float u,float v,GzColor& color)
{
	if ( (((int) floor(u*16.f))+ (int) floor(v*16.f)) % 2 == 0 ) {
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

