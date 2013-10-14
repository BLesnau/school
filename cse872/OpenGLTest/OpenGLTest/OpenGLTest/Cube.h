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
   CCube( vec3 pos, vec3 rot, vec3 size, float mass, BOOL bShowOneSide = FALSE, BOOL bStatic = FALSE );
   ~CCube();

   void InitGL();
   void RenderGL( GLuint program  );

   void UpdateVelocity( float dt );
   void UpdatePosition( float dt );

   static void AddCollisionImpulse(CCube* cube1, CCube* cube2, vec3 &hitPoint, vec3 &normal, float dt, float penetration);

   vec3  m_c;	   // Center Of Cube
   vec4  m_u[3];	// Local x-y-z Axes
   vec3  m_e;	   // Positive halfwidths along each axis

   mat4  m_matWorld;
   quat  m_rot;

   float m_mass;
   mat4	m_boxInertia;
   mat3	m_boxInvInertia;
   mat4	m_invInertia;

   vec3  m_linVelocity;
   vec4  m_angVelocity;

   vec3  m_forces;
   vec4  m_torques;
   float m_radius;	
   float m_rwaMotion;
   bool  m_awake;

private:
   void Quad(int a, int b, int c, int d, int& index);
   void ColorCube();

   void UpdateMatrix();
   void InitCube( vec3 pos, vec3 rot, vec3 size, float mass, BOOL bShowOneSide, BOOL bStatic );

   GLuint m_vao;
   GLuint m_buffer;

   point4   m_origPoints[36];
   point4   m_points[36];
   color4   m_colors[36];
   vec3     m_normals[36];
   /*point4*   m_origPoints;
   point4*   m_points;
   color4*   m_colors;
   vec3*     m_normals;*/

   BOOL m_bStatic;
   BOOL m_bShowOneSide;
};