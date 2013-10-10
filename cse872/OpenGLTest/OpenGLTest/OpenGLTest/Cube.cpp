#include "stdafx.h"

#include "Cube.h"
#include "ShaderWnd\ShaderWnd.h"

const int NumVertices = 36;

// Vertices of a unit cube centered at origin, sides aligned with axes
//point4 vertex_positions[8] = {
//   point4( -0.5, -0.5,  0.5, 1.0 ),
//   point4( -0.5,  0.5,  0.5, 1.0 ),
//   point4(  0.5,  0.5,  0.5, 1.0 ),
//   point4(  0.5, -0.5,  0.5, 1.0 ),
//   point4( -0.5, -0.5, -0.5, 1.0 ),
//   point4( -0.5,  0.5, -0.5, 1.0 ),
//   point4(  0.5,  0.5, -0.5, 1.0 ),
//   point4(  0.5, -0.5, -0.5, 1.0 )
//};
//
//// Vertices of a unit cube centered at origin, sides aligned with axes
//point4 vertex_positions2[8] = {
//   point4( 0.5, 0.5,  1.5, 1.0 ),
//   point4( 0.5,  1.5,  1.5, 1.0 ),
//   point4(  1.5,  1.5,  1.5, 1.0 ),
//   point4(  1.5, 0.5,  1.5, 1.0 ),
//   point4( 0.5, 0.5, 0.5, 1.0 ),
//   point4( 0.5,  1.5, 0.5, 1.0 ),
//   point4(  1.5,  1.5, 0.5, 1.0 ),
//   point4(  1.5, 0.5, 0.5, 1.0 )
//};

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
   point4 vertex_positions[8] = {
      point4( -m_e.x, -m_e.y,  m_e.z, 1.0 ),
      point4( -m_e.x,  m_e.y,  m_e.z, 1.0 ),
      point4(  m_e.x,  m_e.y,  m_e.z, 1.0 ),
      point4(  m_e.x, -m_e.y,  m_e.z, 1.0 ),
      point4( -m_e.x, -m_e.y, -m_e.z, 1.0 ),
      point4( -m_e.x,  m_e.y, -m_e.z, 1.0 ),
      point4(  m_e.x,  m_e.y, -m_e.z, 1.0 ),
      point4(  m_e.x, -m_e.y, -m_e.z, 1.0 )
   };

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

//void CCube::Quad2(int a, int b, int c, int d, int& index) 
//{
//   vec3 u = vec3(vertex_positions2[b]-vertex_positions2[a]);	
//   vec3 v = vec3(vertex_positions2[c]-vertex_positions2[a]);
//   vec3 normal = vec3(normalize(cross(u,v)));
//
//   m_normals[index] = normal; m_colors[index] = vertex_colors[a]; m_points[index] = vertex_positions2[a]; index++;
//   m_normals[index] = normal; m_colors[index] = vertex_colors[b]; m_points[index] = vertex_positions2[b]; index++;
//   m_normals[index] = normal; m_colors[index] = vertex_colors[c]; m_points[index] = vertex_positions2[c]; index++;
//   m_normals[index] = normal; m_colors[index] = vertex_colors[a]; m_points[index] = vertex_positions2[a]; index++;
//   m_normals[index] = normal; m_colors[index] = vertex_colors[c]; m_points[index] = vertex_positions2[c]; index++;
//   m_normals[index] = normal; m_colors[index] = vertex_colors[d]; m_points[index] = vertex_positions2[d]; index++;
//}

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

//void CCube::ColorCube2() 
//{
//   int index = 0;
//
//   Quad2( 1, 0, 3, 2, index );
//   Quad2( 2, 3, 7, 6, index );
//   Quad2( 3, 0, 4, 7, index );
//   Quad2( 6, 5, 1, 2, index );
//   Quad2( 4, 5, 6, 7, index );
//   Quad2( 5, 4, 0, 1, index );
//}

CCube::CCube()
{
   InitCube( vec3( 0, 0, 0 ), vec3( 0, 0, 0 ), vec3( 1, 1, 1 ), 1.0f);
}

CCube::CCube( vec3 pos, vec3 rot, vec3 size, float mass )
{
   InitCube( pos, rot, size, mass );
}

void CCube::InitCube( vec3 pos, vec3 rot, vec3 size, float mass )
{
   m_c = pos;
   m_e = size;
   m_e *= 0.5f;

   m_rot = quat( rot );
   normalize( m_rot );

   m_mass = mass;
   m_linVelocity = vec3( 0, 0, 0 );
   m_angVelocity = vec4( 0, 0, 0, 0 );

   m_forces = vec3( 0, 0, 0 );
   m_torques = vec4( 0, 0, 0, 0 );

   // If we want our objects to start awake or sleeping
   //if (g_startSleeping)
   //{
   //   m_rwaMotion = g_sleepEpsilon;
   //   m_awake = false;
   //}
   //else
   //{
   m_rwaMotion = 2 * g_sleepEpsilon;
   //m_awake = true;
   //}

   m_radius = sqrtf(size.x*size.x + size.y*size.y + size.z*size.z) + 0.4f;

   UpdateMatrix();

   ColorCube();
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

void CCube::UpdateVelocity( float dt )
{
   //AddForce
   auto gravity = vec3( 0, -9.8f * m_mass, 0 );
   m_forces += gravity;
   /*vec3 v3 = cross( (m_c - m_c), gravity );
   vec3 v4( v3.x, v3.y, v3.z, 0);
   m_torques += */

   m_angVelocity += ((m_torques * m_invInertia) * dt);

   m_linVelocity += ((m_forces / m_mass) * dt);

   const float damping = 0.98f;
   m_linVelocity *= powf(damping, dt);
   m_angVelocity *= powf(damping, dt);

   UpdateMatrix();

   float motion =  dot(m_linVelocity, m_linVelocity) + dot(m_angVelocity, m_angVelocity);

   float bias = 0.96f;
   m_rwaMotion = bias*m_rwaMotion + (1-bias)*motion;
   if (m_rwaMotion> 50.0f) m_rwaMotion = 5.0f;
}

void CCube::UpdatePosition( float dt )
{
   m_c += (m_linVelocity * dt);

   vec4 angVel = m_angVelocity;

   quat Qvel = 0.5f * m_rot * quat(angVel.x, angVel.y, angVel.z, 0);
   m_rot = m_rot + (Qvel * dt);
   normalize( m_rot );

   m_forces	= vec3( 0, 0, 0 );
   m_torques = vec4( 0, 0, 0, 0 );

   UpdateMatrix();
}

void CCube::UpdateMatrix()
{
   mat4 matR = mat4_cast( m_rot );

   m_u[0] = vec4( 1, 0, 0, 0 );
   m_u[1] = vec4( 0, 1, 0, 0 );
   m_u[2] = vec4( 0, 0, 1, 0 );

   m_u[0] = matR * m_u[0];
   m_u[1] = matR * m_u[1];
   m_u[2] = matR * m_u[2];
   normalize( m_u[0] );
   normalize( m_u[1] );
   normalize( m_u[2] );

   mat4 matT;
   translate( matT, m_c );
   m_matWorld = matR * matT;

   for( int i=0; i<NumVertices; i++ )
   {
      m_points[i] = m_matWorld * m_points[i];
   }

   vec3 size = m_e * 2.0f;
   float x2 = (size.x * size.x);
   float y2 = (size.y * size.y);
   float z2 = (size.z * size.z);
   float ix = (y2 + z2) * m_mass / 12.0f;
   float iy = (x2 + z2) * m_mass / 12.0f;
   float iz = (x2 + y2) * m_mass / 12.0f;

   m_boxInertia = 
      mat4( ix,   0,    0,    0,
      0,    iy,   0,    0,
      0,    0,    iz,   0,
      0,    0,    0,    1);

   m_invInertia = inverse(matR) * inverse(m_boxInertia) * matR;
}
