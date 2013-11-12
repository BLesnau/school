#pragma once

void vectorCopy(GzCoord & to, GzCoord from);
void vectorSubtract(GzCoord & to, GzCoord a, GzCoord b);
void matrixMult(GzMatrix to, GzMatrix a, GzMatrix b);
void Transform(GzCoord & to, GzMatrix m, GzCoord from);
void vectorCross(GzCoord & to, GzCoord a, GzCoord b);
void normalize(GzCoord & v);
void scaling (GzCoord & to, float a, GzCoord b);
float dot (GzCoord a, GzCoord b);
void loadIdentity(GzMatrix m);
void lineEquations(double *e, double* a, double* b,GzCoord** vertexList, int x_start, int y_start);
void bounding_box(GzCoord ** vertexList, float & xmin, float & ymin, float & xmax, float & ymax, int & x_start, int& y_start, int& x_end, int& y_end);

//Other utilities
short	ctoi(float color);	/* convert float color to GzIntensity short */
void	clean_up_list(int numOutput, GzToken * outputType, GzPointer * outList);
void	allocate_list(int num, GzToken* typeList, GzPointer* inputlist);
void	assemble_vertex_input(int num, GzToken * typeList, GzPointer**  inlist, GzPointer * valuelist, int i);
void	assemble_fragment_input(int num, GzToken * typeList, GzPointer*  fragment_input_list, GzPointer ** list, double *e);

