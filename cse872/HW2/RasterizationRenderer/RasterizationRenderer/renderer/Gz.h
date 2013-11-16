/*
 * Gz.h - include file for rendering library
 * CSE 872 
*/
#pragma once
#include <array>
#include <vector>
/*
 * universal constants
 */
#define GZ_SUCCESS      0
#define GZ_FAILURE      1

/*
 * display classes
 */
typedef enum {GZ_RGBAZ_DISPLAY}  GzDisplayClass;

/*
 * renderer classes
 */
typedef enum {GZ_Z_BUFFER_RENDER}  GzRenderClass;


typedef void    *GzPointer;
typedef std::array<float,3>   GzColor;
typedef short   GzIntensity;		/* 0-4095 in lower 12-bits for RGBA */
typedef int	    GzDepth;		/* signed z for clipping */
typedef std::array<float, 3> GzCoord;
typedef std::array<float, 2> GzTextureIndex;
typedef std::array<int, 2> GzPoint;
typedef float	GzMatrix[4][4];
typedef int		GzDepth;	/* z is signed for clipping */
typedef	int	    (*GzTexture)(float u, float v, GzColor &color);	/* pointer to texture sampling method */

/*
 * name list tokens
 */
/* triangle vert attributes */
typedef enum {GZ_NULL_TOKEN, GZ_POSITION, GZ_NORMAL, GZ_COLOR, GZ_TEXTURE_INDEX,

GZ_DIRECTIONAL_LIGHT	=	79	/* directional light */,
GZ_AMBIENT_LIGHT	=	78	/* ambient light type */,

GZ_AMBIENT_COEFFICIENT	=	1001	/* Ka material property */,
GZ_DIFFUSE_COEFFICIENT	=	1002	/* Kd material property */,
GZ_SPECULAR_COEFFICIENT	=	1003	/* Ks material property */,
GZ_DISTRIBUTION_COEFFICIENT =	1004	/* specular power of material */,
GZ_TEXTURE_MAP	=		1010	/* pointer to texture routine */,

GZ_VERTEX_SHADER,
GZ_FRAGMENT_SHADER
} GzToken;

class GzRender;
typedef void (*GzShader)(int numInput, GzToken * inputType, GzPointer *inList, int& numOutput, GzToken * &outputType, GzPointer *&outList, GzRender * renderer);

/* array indices for color vector */
enum { RED, GREEN, BLUE};

/* array indices for position vector */
enum {X, Y, Z};

/* array indices for texture coords */
enum {U, V};

typedef	struct {
  GzIntensity    red;	
  GzIntensity    green;
  GzIntensity    blue;
  GzIntensity    alpha;
  GzDepth	 	z;
} GzPixel;

/* define a display type */
class GzDisplay {
public:
	unsigned short	xres;
	unsigned short	yres;
	GzDisplayClass	disp_class;
	short		open;
	GzPixel		*fbuf;		/* frame buffer array */

	GzDisplay(GzDisplayClass disp_class,int xRes,int yRes);
	~GzDisplay ();
	int GetParams(int	*xRes, int		*yRes, GzDisplayClass	*disp_class);
	int Init();
	int Put(int		i, int		j, GzIntensity	r, GzIntensity	g, GzIntensity	b, GzIntensity	a, GzDepth		z);
	int Get(int		i,int		j,GzIntensity	*r,GzIntensity	*g,GzIntensity	*b,GzIntensity	*a,GzDepth		*z);
	int Flush();
	BYTE *image;

	friend class GzRender;

};

#define	MAXXRES	1024		/* put some bounds on size in case of error */
#define	MAXYRES	1024

#define	ARRAY(x,y)	(x+(y*display->xres))	/* simplify fbuf indexing */ 

/* Camera defaults */
#define	DEFAULT_FOV		35.0
#define	DEFAULT_IM_Z	(-10.0)  /* world coords for image plane origin */
#define	DEFAULT_IM_Y	(5.0)    /* default look-at point = 0,0,0 */
#define	DEFAULT_IM_X	(-10.0)

#define	DEFAULT_AMBIENT	{0.1, 0.1, 0.1}
#define	DEFAULT_DIFFUSE	{0.7, 0.6, 0.5}
#define	DEFAULT_SPECULAR	{0.2, 0.3, 0.4}
#define	DEFAULT_SPEC		32

#define	MATLEVELS	10		/* how many matrix pushes allowed */
#define	MAX_LIGHTS	10		/* how many lights allowed */

/*
 * Gz camera definition - for making Xiw with rotations, scaling, translations.  
 * Perspective projection is separate Xpi. 
 */
typedef struct  GzCamera
{
        GzMatrix        Xiw;  		/* xform from world to image space */
        GzMatrix        Xpi;            /* perspective projection xform */
        GzCoord         position;       /* position of image plane origin */
        GzCoord         lookat;         /* position of look-at-point */
        GzCoord         worldup;        /* world up-vector (almost screen up) */
        float           FOV;            /* horizontal field of view */
} GzCamera;

typedef struct  GzLight
{
        GzCoord        direction; 	/* vector from surface to light */
        GzColor        color;		/* light color intensity */
} GzLight;

class GzRender {			/* define a renderer */
public:
  GzRenderClass		r_class;
  GzDisplay		*display;
  short		open;
  GzCamera		camera;
  short		matlevel;	/* top of stack - current xform */
  GzMatrix		Ximage[MATLEVELS];	/* stack of xforms (Xsm) */
  GzMatrix		Xnorm[MATLEVELS];	/* xforms for norms (Xim) */
  GzMatrix		Xsp;		/* NDC to screen (pers-to-screen) */

  int			numlights;
  GzLight		lights[MAX_LIGHTS];
  GzLight		ambientlight;
  GzColor		Ka, Kd, Ks;
  float		spec;		/* specular power */
  GzTexture		tex_fun;  /* tex_fun(float u, float v, GzColor color) */
  GzShader		vertex_shader;
  GzShader		fragment_shader;


  GzRender(GzRenderClass	r_class,GzDisplay	*display);
  ~GzRender();
  int Begin();
  int PutAttribute(int numAttributes, GzToken	*nameList, GzPointer	*valueList);
  int PutCamera(GzCamera	*camera);
  int PushMatrix(GzMatrix	matrix);
  int PopMatrix();

  int DrawTriangles(int numTriangles, int		numParts,	/* how many names and values */ GzToken	*nameList, GzPointer	*valueList);
  void Rasterize(int num, GzToken * typeList, GzPointer * list[3]);
};

#include "utility.h"