#include "stdafx.h"

#include "Cube.h"
#include "ShaderWnd\ShaderWnd.h"

const int NumVertices = 36;

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertex_positions[8] = {
   point4( -0.5, -0.5,  0.5, 1.0 ),
   point4( -0.5,  0.5,  0.5, 1.0 ),
   point4(  0.5,  0.5,  0.5, 1.0 ),
   point4(  0.5, -0.5,  0.5, 1.0 ),
   point4( -0.5, -0.5, -0.5, 1.0 ),
   point4( -0.5,  0.5, -0.5, 1.0 ),
   point4(  0.5,  0.5, -0.5, 1.0 ),
   point4(  0.5, -0.5, -0.5, 1.0 )
};

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertex_positions2[8] = {
   point4( 0.5, 0.5,  1.5, 1.0 ),
   point4( 0.5,  1.5,  1.5, 1.0 ),
   point4(  1.5,  1.5,  1.5, 1.0 ),
   point4(  1.5, 0.5,  1.5, 1.0 ),
   point4( 0.5, 0.5, 0.5, 1.0 ),
   point4( 0.5,  1.5, 0.5, 1.0 ),
   point4(  1.5,  1.5, 0.5, 1.0 ),
   point4(  1.5, 0.5, 0.5, 1.0 )
};

// RGBA colors
color4 vertex_colors[8] = {
   color4( 0.0, 0.0, 0.0, 1.0 ),  // black
   color4( 1.0, 0.0, 0.0, 1.0 ),  // red
   color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
   color4( 0.0, 1.0, 0.0, 1.0 ),  // green
   color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
   color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
   color4( 1.0, 1.0, 1.0, 1.0 ),  // white
   color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
};

// quad() generates two triangles for each face and assigns colors to the vertices
void CCube::Quad(int a, int b, int c, int d, int& index) 
{
   vec3 u = vec3(vertex_positions[b]-vertex_positions[a]);	
   vec3 v = vec3(vertex_positions[c]-vertex_positions[a]);
   vec3 normal = vec3(normalize(cross(u,v)));

   m_normals[index] = normal; m_colors[index] = vertex_colors[a]; m_points[index] = vertex_positions[a]; index++;
   m_normals[index] = normal; m_colors[index] = vertex_colors[b]; m_points[index] = vertex_positions[b]; index++;
   m_normals[index] = normal; m_colors[index] = vertex_colors[c]; m_points[index] = vertex_positions[c]; index++;
   m_normals[index] = normal; m_colors[index] = vertex_colors[a]; m_points[index] = vertex_positions[a]; index++;
   m_normals[index] = normal; m_colors[index] = vertex_colors[c]; m_points[index] = vertex_positions[c]; index++;
   m_normals[index] = normal; m_colors[index] = vertex_colors[d]; m_points[index] = vertex_positions[d]; index++;
}

void CCube::Quad2(int a, int b, int c, int d, int& index) 
{
   vec3 u = vec3(vertex_positions2[b]-vertex_positions2[a]);	
   vec3 v = vec3(vertex_positions2[c]-vertex_positions2[a]);
   vec3 normal = vec3(normalize(cross(u,v)));

   m_normals[index] = normal; m_colors[index] = vertex_colors[a]; m_points[index] = vertex_positions2[a]; index++;
   m_normals[index] = normal; m_colors[index] = vertex_colors[b]; m_points[index] = vertex_positions2[b]; index++;
   m_normals[index] = normal; m_colors[index] = vertex_colors[c]; m_points[index] = vertex_positions2[c]; index++;
   m_normals[index] = normal; m_colors[index] = vertex_colors[a]; m_points[index] = vertex_positions2[a]; index++;
   m_normals[index] = normal; m_colors[index] = vertex_colors[c]; m_points[index] = vertex_positions2[c]; index++;
   m_normals[index] = normal; m_colors[index] = vertex_colors[d]; m_points[index] = vertex_positions2[d]; index++;
}

void CCube::ColorCube() 
{
   int index = 0;

   Quad( 1, 0, 3, 2, index );
   Quad( 2, 3, 7, 6, index );
   Quad( 3, 0, 4, 7, index );
   Quad( 6, 5, 1, 2, index );
   Quad( 4, 5, 6, 7, index );
   Quad( 5, 4, 0, 1, index );
}

void CCube::ColorCube2() 
{
   int index = 0;

   Quad2( 1, 0, 3, 2, index );
   Quad2( 2, 3, 7, 6, index );
   Quad2( 3, 0, 4, 7, index );
   Quad2( 6, 5, 1, 2, index );
   Quad2( 4, 5, 6, 7, index );
   Quad2( 5, 4, 0, 1, index );
}

CCube::CCube(BOOL other)
{
   if(other)
   {
      ColorCube2();
   }
   else
   {
      ColorCube();
   }
}

CCube::~CCube()
{
}

//void CCube::InitGL( GLuint& program )
//{
//   GLuint vao;
//   glGenVertexArrays(1, &vao);
//   glBindVertexArray(vao);
//
//   GLuint buffer;
//   glGenBuffers( 1, &buffer );
//   glBindBuffer( GL_ARRAY_BUFFER, buffer );
//   glBufferData( GL_ARRAY_BUFFER, sizeof(m_points) + sizeof(m_colors), NULL, GL_STATIC_DRAW );
//   glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(m_points), m_points );
//   glBufferSubData( GL_ARRAY_BUFFER, sizeof(m_points), sizeof(m_colors), m_colors );
//
//   GLuint vPosition = glGetAttribLocation( program, "vPosition" );
//   glEnableVertexAttribArray( vPosition );
//   glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
//   GLuint vColor = glGetAttribLocation( program, "vColor" );
//   glEnableVertexAttribArray( vColor );
//   glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(m_points)) );
//}
//
//void CCube::RenderGL( GLuint program  )
//{
//
//   
//
//   glDrawArrays( GL_TRIANGLES, 0, NumVertices );
//}

void CCube::InitGL()
{
   glGenVertexArrays( 1, &m_vao );
   glGenBuffers( 1, &m_buffer );
}

void CCube::RenderGL( GLuint program )
{
   glBindVertexArray( m_vao );
   glBindBuffer( GL_ARRAY_BUFFER, m_buffer );

   glBufferData( GL_ARRAY_BUFFER, sizeof(m_points) + sizeof(m_colors), NULL, GL_STATIC_DRAW );
   glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(m_points), m_points );
   glBufferSubData( GL_ARRAY_BUFFER, sizeof(m_points), sizeof(m_colors), m_colors );

   GLuint vPosition = glGetAttribLocation( program, "vPosition" );
   glEnableVertexAttribArray( vPosition );
   glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
   GLuint vColor = glGetAttribLocation( program, "vColor" );
   glEnableVertexAttribArray( vColor );
   glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(m_points)) );

   glDrawArrays( GL_TRIANGLES, 0, NumVertices );
}
