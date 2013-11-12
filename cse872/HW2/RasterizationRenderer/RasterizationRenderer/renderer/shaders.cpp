#include "stdafx.h"
#include "Gz.h"
#include <math.h>

//a simple shading subroutine based on normal direction
void shade(GzCoord	norm, GzCoord	&color)
{
	GzCoord	light;
	float		coef;

	light[0] = 0.707f;
	light[1] = 0.5f;
	light[2] = 0.5f;

	coef = light[0]*norm[0] + light[1]*norm[1] + light[2]*norm[2];
	if (coef < 0) 	coef *= -1.f;
	if (coef > 1.0)	coef = 1.0f;
	color[0] = coef*0.95f;
	color[1] = coef*0.65f;
	color[2] = coef*0.88f;
}

//full blown shading subroutine
void shading(GzColor& output, GzCoord normal,  float u, float v, GzRender *renderer)
{
	normalize(normal);

	GzColor color;

	GzTexture tex_fun = renderer->tex_fun;
	if (tex_fun) {
		tex_fun(u, v, color);
	}

	for (int i = 0; i < 3; i++) {
		if (tex_fun) {
			output[i] = color[i]*renderer->ambientlight.color[i];
		} else 
			output[i] = renderer->Ka[i]*renderer->ambientlight.color[i];
	}
	for (int j = 0; j < renderer->numlights; j++) {
		GzCoord r;
		float NdotL = dot(normal,renderer->lights[j].direction);
		if (NdotL < 0)
			continue;
		scaling(r, 2.f* NdotL ,normal);
		vectorSubtract(r, r, renderer->lights[j].direction);
		for (int i = 0; i < 3; i++) {
			if (tex_fun)
				output[i] += color[i] * renderer->lights[j].color[i] * pow(-r[2],renderer->spec)
				+ color[i] * renderer->lights[j].color[i]* NdotL;
			else
				output[i] += renderer->Ks[i] * renderer->lights[j].color[i] * pow(-r[2], renderer->spec)
				+ renderer->Kd[i] * renderer->lights[j].color[i]* NdotL;
		}
	}
}


void simple_vertex_shader(int numInput, GzToken *typeInput, GzPointer *inList, int &numOutput, GzToken *&typeOutput, GzPointer *&outList, GzRender *renderer)
{
	//uniform variables are simulated by the members of renderer
	//in variables
	GzCoord * vertex_pos;
	GzCoord * in_vertex_normal;
	// simulate linking input variables
	for (int i = 0; i < numInput; i++)
	{
		switch (typeInput[i]) {
		case GZ_NULL_TOKEN:
			break;
		case GZ_POSITION:
			vertex_pos = (GzCoord *) inList[i];
			break;
		case GZ_NORMAL:
			in_vertex_normal = (GzCoord *) inList[i];
			break;
		default:
			break;
		}
	}

	// simulate output variables
	GzCoord * out_vertex_pos;
	GzColor * out_color;
	numOutput = 2;
	typeOutput = new GzToken[numOutput];
	outList = new GzPointer[numOutput];
	typeOutput[0]= GZ_POSITION;
	outList[0]= out_vertex_pos = new GzCoord;
	typeOutput[1] = GZ_COLOR;
	outList[1]= out_color = new GzColor;
	//actual shader
	//renderer's members treated as "uniform" variables
	Transform(*out_vertex_pos, renderer->Ximage[renderer->matlevel], *vertex_pos); 
	shade(*in_vertex_normal, *out_color);
}

void simple_fragment_shader(int numInput, GzToken *typeInput, GzPointer *inList, int &numOutput, GzToken * &typeOutput, GzPointer * &outList, GzRender *renderer)
{
	GzCoord * color;
	GzCoord * out_color;
	// simulate linking input variables
	for (int i = 0; i < numInput; i++)
	{
		switch (typeInput[i]) {
		case GZ_NULL_TOKEN:
			break;
		case GZ_COLOR:
			color = (GzColor*) inList[i];
			break;
		default:
			break;
		}
	}
	// simulate output variables
	numOutput = 1;
	typeOutput = new GzToken[1];
	outList = new GzPointer[1];
	typeOutput[0] = GZ_COLOR;
	outList[0]= out_color = new GzColor;
	
	//actual shader
	vectorCopy(*out_color, *color);
}

void gouraud_vertex_shader(int numInput, GzToken *typeInput, GzPointer *inList, int &numOutput, GzToken *&typeOutput, GzPointer *&outList, GzRender *renderer)
{
	//uniform variables are simulated by the members of renderer
	//in variables
	GzCoord * vertex_pos;
	GzCoord * in_vertex_normal;
	// simulate linking input variables
	for (int i = 0; i < numInput; i++)
	{
		switch (typeInput[i]) {
		case GZ_NULL_TOKEN:
			break;
		case GZ_POSITION:
			vertex_pos = (GzCoord *) inList[i];
			break;
		case GZ_NORMAL:
			in_vertex_normal = (GzCoord *) inList[i];
			break;
		default:
			break;
		}
	}

	// simulate output variables
	GzCoord * out_vertex_pos;
	GzColor * out_color;
	numOutput = 2;
	typeOutput = new GzToken[numOutput];
	outList = new GzPointer[numOutput];
	typeOutput[0]= GZ_POSITION;
	outList[0]= out_vertex_pos = new GzCoord;
	typeOutput[1] = GZ_COLOR;
	outList[1]= out_color = new GzColor;
	//actual shader
	//renderer's members treated as "uniform" variables
	Transform(*out_vertex_pos, renderer->Ximage[renderer->matlevel], *vertex_pos); 
	GzCoord vertex_normal;
	Transform(vertex_normal, renderer->Xnorm[renderer->matlevel], *in_vertex_normal);
	shading( *out_color, vertex_normal, 0.f, 0.f, renderer);
}


void phong_vertex_shader(int numInput, GzToken *typeInput, GzPointer *inList, int &numOutput, GzToken *&typeOutput, GzPointer *&outList, GzRender *renderer)
{
	//uniform variables are simulated by the members of renderer
	//in variables
	GzCoord * vertex_pos;
	GzCoord * in_vertex_normal;
	GzTextureIndex * in_texture;
	// simulate linking input variables
	for (int i = 0; i < numInput; i++)
	{
		switch (typeInput[i]) {
		case GZ_NULL_TOKEN:
			break;
		case GZ_POSITION:
			vertex_pos = (GzCoord *) inList[i];
			break;
		case GZ_NORMAL:
			in_vertex_normal = (GzCoord *) inList[i];
			break;
		case GZ_TEXTURE_INDEX:
			in_texture = (GzTextureIndex*) inList[i];
			break;
		default:
			break;
		}
	}

	// simulate output variables
	GzCoord * out_vertex_pos;
	GzColor * out_normal;
	GzTextureIndex *out_texture;
	numOutput = 3;
	typeOutput = new GzToken[numOutput];
	outList = new GzPointer[numOutput];
	typeOutput[0]= GZ_POSITION;
	outList[0]= out_vertex_pos = new GzCoord;
	typeOutput[1] = GZ_NORMAL;
	outList[1]= out_normal = new GzCoord;
	typeOutput[2] = GZ_TEXTURE_INDEX;
	outList[2]= out_texture	= new GzTextureIndex;
	//actual shader
	//renderer's members treated as "uniform" variables
	Transform(*out_vertex_pos, renderer->Ximage[renderer->matlevel], *vertex_pos); 
	Transform(* out_normal, renderer->Xnorm[renderer->matlevel], *in_vertex_normal);
	out_texture->at(0) = in_texture->at(0);
	out_texture->at(1) = in_texture->at(1);
}

void phong_fragment_shader(int numInput, GzToken *typeInput, GzPointer *inList, int &numOutput, GzToken * &typeOutput, GzPointer * &outList, GzRender *renderer)
{
	GzCoord * normal;
	GzTextureIndex  * texture;
	GzCoord * out_color;
	// simulate linking input variables
	for (int i = 0; i < numInput; i++)
	{
		switch (typeInput[i]) {
		case GZ_NORMAL:
			normal = (GzCoord*) inList[i];
			break;
		case GZ_TEXTURE_INDEX:
			texture = (GzTextureIndex*) inList[i];
		default:
			break;
		}
	}
	// simulate output variables
	numOutput = 1;
	typeOutput = new GzToken[1];
	outList = new GzPointer[1];
	typeOutput[0] = GZ_COLOR;
	outList[0]= out_color = new GzColor;

	//actual shader
	shading(*out_color, *normal, texture->at(0), texture->at(1), renderer);
}
