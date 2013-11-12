#include "stdafx.h"
#include "Gz.h"

void vectorCopy(GzCoord & to, GzCoord from)
{
	to[0] = from[0];
	to[1] = from[1];
	to[2] = from[2];
}

void vectorSubtract(GzCoord & to, GzCoord a, GzCoord b)
{
	to[0] = a[0] - b[0];
	to[1] = a[1] - b[1];
	to[2] = a[2] - b[2];
}

void matrixMult(GzMatrix to, GzMatrix a, GzMatrix b)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			to[i][j] = 0.;
			for (int k = 0; k < 4; k++)
				to[i][j] += a[i][k]*b[k][j];
		}
	}
}

void Transform(GzCoord & to, GzMatrix m, GzCoord from)
{
	float t[4], f[4];

	f[0] = from[0];
	f[1] = from[1];
	f[2] = from[2];
	f[3] = 1.;

	for (int i = 0; i < 4; i++) {
		t[i] = 0.;
		for (int j = 0; j < 4; j++) {
			t[i] += m[i][j] * f[j];
		}
	}

	to[0] = t[0]/t[3];
	to[1] = t[1]/t[3];
	to[2] = t[2]/t[3];
}


void vectorCross(GzCoord & to, GzCoord a, GzCoord b)
{
	to[0] = a[1]*b[2] - a[2]*b[1];
	to[1] = a[2]*b[0] - a[0]*b[2];
	to[2] = a[0]*b[1] - a[1]*b[0];
}

void normalize(GzCoord & v)
{
	float norm = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);

	v[0]/=norm;
	v[1]/=norm;
	v[2]/=norm;
}

void scaling (GzCoord & to, float a, GzCoord b)
{
	to[0] = a * b[0];
	to[1] = a * b[1];
	to[2] = a * b[2];
}

float dot (GzCoord a, GzCoord b)
{
	return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}

///////
void loadIdentity(GzMatrix m)
{
	m[0][0] = 1.; m[0][1] = 0.; m[0][2] = 0.; m[0][3] = 0.;
	m[1][0] = 0.; m[1][1] = 1.; m[1][2] = 0.; m[1][3] = 0.;
	m[2][0] = 0.; m[2][1] = 0.; m[2][2] = 1.; m[2][3] = 0.;
	m[3][0] = 0.; m[3][1] = 0.; m[3][2] = 0.; m[3][3] = 1.;
}



/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
	return(short)((int)(color * ((1 << 12) - 1)));
}

//cleaning up the content of a list
void clean_up_list(int numOutput, GzToken * outputType, GzPointer * outList)
{
	for (int j = 0; j < numOutput; j++) {
		switch (outputType[j]) {
		case GZ_POSITION:
		case GZ_NORMAL:
			delete (GzCoord*) (outList[j]);
			break;
		case GZ_COLOR:
			delete (GzColor*) (outList[j]);
			break;
		case GZ_TEXTURE_INDEX:
			delete (GzTextureIndex*) (outList[j]);
			break;
		default:
			break;
		}
	}
	delete [] outList;
}

//allocate based on the type list
void allocate_list(int num, GzToken* typeList, GzPointer* inputlist)
{
	for (int j = 0; j < num; j++) {
		switch (typeList[j]) {
		case GZ_POSITION:
		case GZ_NORMAL:
			inputlist[j] = new GzCoord;
			break;
		case GZ_COLOR:
			inputlist[j] = new GzColor;
			break;
		case GZ_TEXTURE_INDEX:
			inputlist[j] = new GzTextureIndex;
		default:
			break;
		}
	}
}

void bounding_box(GzCoord ** vertexList, float & xmin, float & ymin, float & xmax, float & ymax, int & x_start, int& y_start, int& x_end, int& y_end)
{
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

	x_start = (int) ceil(xmin);
	y_start = (int) ceil(ymin);
	x_end = (int) ceil(xmax);
	y_end = (int) ceil(ymax);
}

void lineEquations(double *e, double* a, double* b,GzCoord** vertexList, int x_start, int y_start)
{
	//Todo: modify it if you want to use the equations to produce linear interpolation coefficients.
	float sign = ((*(vertexList[1]))[X] - (*(vertexList[0]))[X]) * ((*(vertexList[2]))[Y] - (*(vertexList[0]))[Y]) - ((*(vertexList[1]))[Y] - (*(vertexList[0]))[Y])* ((*(vertexList[2]))[X] - (*(vertexList[0]))[X]);

	if (sign>0)
		sign = 1.f;
	else
		sign = -1.f;

	//line equations for all three edges of the triangle: e(x,y) = a x + by + c 
	b[2] = (*(vertexList[1]))[X] - (*(vertexList[0]))[X];
	a[2] = (*(vertexList[0]))[Y] - (*(vertexList[1]))[Y];
	a[2] *= sign;
	b[2] *= sign;
	e[2] = a[2] * ((float)x_start-(*(vertexList[0]))[X]) + b[2] * ((float)y_start-(*(vertexList[0]))[Y]);

	b[0] = (*(vertexList[2]))[X] - (*(vertexList[1]))[X];
	a[0] = (*(vertexList[1]))[Y] - (*(vertexList[2]))[Y];
	a[0] *= sign;
	b[0] *= sign;
	e[0] = a[0] * ((float)x_start-(*(vertexList[1]))[X]) + b[0] * ((float)y_start-(*(vertexList[1]))[Y]);


	b[1] = (*(vertexList[0]))[X] - (*(vertexList[2]))[X];
	a[1] = (*(vertexList[2]))[Y] - (*(vertexList[0]))[Y];
	a[1] *= sign;
	b[1] *= sign;
	e[1] = a[1] * ((float)x_start-(*(vertexList[2]))[X]) + b[1] * ((float)y_start-(*(vertexList[2]))[Y]);
}

void assemble_vertex_input(int num, GzToken * typeList, GzPointer**  inList, GzPointer * valueList, int i)
{	
	//assemble input variables from "VBO"'s
	for (int j = 0; j < num; j++) {
		switch (typeList[j]) {
		case GZ_NULL_TOKEN:
			break;
		case GZ_POSITION:
			inList[0][j] = (GzPointer*) &(*(std::vector<GzCoord> *) (valueList[j]))[3*i];
			inList[1][j] = (GzPointer*) &(*(std::vector<GzCoord> *) (valueList[j]))[3*i+1];
			inList[2][j] = (GzPointer*) &(*(std::vector<GzCoord> *) (valueList[j]))[3*i+2];
			break;
		case GZ_NORMAL:
			inList[0][j] = (GzPointer*) &(*(std::vector<GzCoord> *) valueList[j])[3*i];
			inList[1][j] = (GzPointer*) &(*(std::vector<GzCoord> *) valueList[j])[3*i+1];
			inList[2][j] = (GzPointer*) &(*(std::vector<GzCoord> *) valueList[j])[3*i+2];
			break;
		case GZ_TEXTURE_INDEX:
			inList[0][j] = (GzPointer*) &(*(std::vector<GzTextureIndex> *) valueList[j])[3*i];
			inList[1][j] = (GzPointer*) &(*(std::vector<GzTextureIndex> *) valueList[j])[3*i+1];
			inList[2][j] = (GzPointer*) &(*(std::vector<GzTextureIndex> *) valueList[j])[3*i+2];
			break;
		default:
			break;
		}
	}
}

void assemble_fragment_input(int num, GzToken * typeList, GzPointer*  fragment_input_list, GzPointer ** list, double *e)
{	
	//Todo: linear interpolate all vertex shader output in list to prepare the input in fragment shader
	for (int j = 0; j < num; j++) {
		switch (typeList[j]) {
		case GZ_POSITION:
		case GZ_NORMAL:
			(*( (GzCoord*) (fragment_input_list[j])))[0] = (*((GzCoord*) (list[0][j])))[0];
			(*( (GzCoord*) (fragment_input_list[j])))[1] = (*((GzCoord*) (list[0][j])))[1];
			(*( (GzCoord*) (fragment_input_list[j])))[2] = (*((GzCoord*) (list[0][j])))[2];
			break;
		case GZ_COLOR:
			(*( (GzColor*) (fragment_input_list[j])))[0] = (*((GzColor*) (list[0][j])))[0];
			(*( (GzColor*) (fragment_input_list[j])))[1] = (*((GzColor*) (list[0][j])))[1];
			(*( (GzColor*) (fragment_input_list[j])))[2] = (*((GzColor*) (list[0][j])))[2];
			break;
		case GZ_TEXTURE_INDEX:
			(*( (GzTextureIndex*) (fragment_input_list[j])))[0] = (*((GzTextureIndex*) (list[0][j])))[0];
			(*( (GzTextureIndex*) (fragment_input_list[j])))[1] = (*((GzTextureIndex*) (list[0][j])))[1];
			break;
		default:
			break;
		}
	}

}
