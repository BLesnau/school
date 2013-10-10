#pragma once

#include "ShaderWnd\external\glew-1.9.0\include\GL\glew.h"
#include "ShaderWnd/external/glm/glm.hpp"
#include "ShaderWnd/external/glm/gtc/quaternion.hpp"
#include "Helpers.h"

using namespace glm;

const static float g_sleepEpsilon = 0.05f;

class CCube
{
public:
   CCube();
   CCube( vec3 pos, vec3 rot, vec3 size, float mass );
   ~CCube();

   void InitGL();
   void RenderGL( GLuint program  );

   void UpdateVelocity( float dt );
   void UpdatePosition( float dt );

   vec3  m_c;	   // Center Of Cube
   vec4  m_u[3];	// Local x-y-z Axes
   vec3  m_e;	   // Positive halfwidths along each axis

   mat4  m_matWorld;
   quat  m_rot;

   float m_mass;
   mat4	m_boxInertia;
   mat4	m_boxInvInertia;
   mat4	m_invInertia;

   vec3  m_linVelocity;
   vec3  m_angVelocity;

   vec3  m_forces;
   vec3  m_torques;
   float m_radius;	
   float m_rwaMotion;

private:
   void Quad(int a, int b, int c, int d, int& index);
   void ColorCube();

   //void Quad2(int a, int b, int c, int d, int& index);
   //void ColorCube2();

   void UpdateMatrix();

   GLuint m_vao;
   GLuint m_buffer;

   point4   m_points[36];
   color4   m_colors[36];
   vec3     m_normals[36];
};