#pragma once

#include "physicsobject.h"
#include "ShaderWnd\external\glew-1.9.0\include\GL\glew.h"

class CCube //: public CPhysicsObject
{
public:
   CCube(BOOL other);
   ~CCube();

   void InitGL();
   void RenderGL( GLuint program  );

private:
   void Quad(int a, int b, int c, int d, int& index);
   void ColorCube();

   void Quad2(int a, int b, int c, int d, int& index);
   void ColorCube2();

   GLuint m_vao;
   GLuint m_buffer;

   point4   m_points[36];
   color4   m_colors[36];
   vec3     m_normals[36];
};