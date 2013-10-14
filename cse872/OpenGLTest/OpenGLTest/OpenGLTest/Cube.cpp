#include "stdafx.h"

#include "Cube.h"
#include "ShaderWnd\ShaderWnd.h"
#include "CollisionManager.h"

const int NumVertices = 36;
float g_friction = 0.6f;
bool g_positionCorrection   = true;  // Sinking fix

const bool g_startSleeping = false;

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

   for( int i = 0; i<NumVertices; i++ )
   {
      m_origPoints[i] = m_points[i];
   }
}

void CCube::ColorCube() 
{
   int index = 0;

   Quad( 6, 5, 1, 2, index );

   if( m_bShowOneSide )
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

      m_normals[index] = m_normals[0]; m_colors[index] = vertex_colors[5]; m_points[index] = vertex_positions[5]; index++;

      m_origPoints[index-1] = m_points[index-1];

      return;
   }

   Quad( 1, 0, 3, 2, index );
   Quad( 2, 3, 7, 6, index );
   Quad( 3, 0, 4, 7, index );
   Quad( 4, 5, 6, 7, index );
   Quad( 5, 4, 0, 1, index );
}

CCube::CCube()
{
   InitCube( vec3( 0, 0, 0 ), vec3( 0, 0, 0 ), vec3( 0, 0, 0 ), 1.0f, FALSE, FALSE );
}

CCube::CCube( vec3 pos, vec3 rot, vec3 size, float mass, BOOL bShowOneSide, BOOL bStatic )
{
   InitCube( pos, rot, size, mass, bShowOneSide, bStatic );
}

void CCube::InitCube( vec3 pos, vec3 rot, vec3 size, float mass, BOOL bShowOneSide, BOOL bStatic )
{
   m_bShowOneSide = bShowOneSide;
   m_bStatic = bStatic;

   /*if( m_bShowOneSide )
   {
      g_numVertices = 5;
   }
   else
   {
      g_numVertices = 36;
   }*/

   /*m_origPoints = new point4[g_numVertices];
   m_points = new point4[g_numVertices];
   m_colors = new color4[g_numVertices];
   m_normals = new vec3[g_numVertices];*/

   m_c = pos;
   m_e = size;
   m_e *= 0.5f;

   m_rot = quat( rot );
   m_rot = normalize( m_rot );

   m_mass = mass;
   m_linVelocity = vec3( 0, 0, 0 );
   m_angVelocity = vec4( 0, 0, 0, 1 );

   m_forces = vec3( 0, 0, 0 );
   m_torques = vec4( 0, 0, 0, 1 );

   // If we want our objects to start awake or sleeping
   if (g_startSleeping)
   {
      m_rwaMotion = g_sleepEpsilon;
      m_awake = false;
   }
   else
   {
      m_rwaMotion = 2 * g_sleepEpsilon;
      m_awake = true;
   }

   m_radius = sqrtf(size.x*size.x + size.y*size.y + size.z*size.z) + 0.4f;

   UpdateMatrix();

   ColorCube();
}

CCube::~CCube()
{
}

void CCube::InitGL()
{
   glGenVertexArrays( 1, &m_vao );
   glGenBuffers( 1, &m_buffer );
}

void CCube::RenderGL( GLuint program )
{
   glBindVertexArray( m_vao );
   glBindBuffer( GL_ARRAY_BUFFER, m_buffer );

   glEnable( GL_CULL_FACE );
   glCullFace( GL_BACK );

   glBufferData( GL_ARRAY_BUFFER, sizeof(m_points) + sizeof(m_colors), NULL, GL_STATIC_DRAW );
   glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(m_points), m_points );
   glBufferSubData( GL_ARRAY_BUFFER, sizeof(m_points), sizeof(m_colors), m_colors );

   GLuint vPosition = glGetAttribLocation( program, "vPosition" );
   glEnableVertexAttribArray( vPosition );
   glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
   GLuint vColor = glGetAttribLocation( program, "vColor" );
   glEnableVertexAttribArray( vColor );
   glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(m_points)) );

   if( m_bShowOneSide )
   {
      glDrawArrays( GL_LINE_LOOP, 0, NumVertices );
   }
   else
   {
      glDrawArrays( GL_TRIANGLES, 0, NumVertices );
   }
}

void CCube::UpdateVelocity( float dt )
{
   if( !m_awake )
   {
      return;
   }

   if( !m_bStatic )
   {
      if (length(m_linVelocity) < 0.0001f)  m_linVelocity = vec3(0,0,0);
      if (length(m_angVelocity) < 0.0001f)  m_angVelocity = vec4(0,0,0,1);

      if( m_awake )
      {     
         auto gravity = vec3( 0, -9.8f * m_mass, 0 );
         m_forces += gravity;
         vec3 v3 = cross( (m_c - m_c), gravity );
         vec4 v4( v3.x, v3.y, v3.z, 0);
         m_torques = m_torques + v4;
      }

      m_angVelocity += ((m_torques * m_invInertia) * dt);

      m_linVelocity += ((m_forces / m_mass) * dt);

      const float damping = 0.98f;
      m_linVelocity *= powf(damping, dt);
      m_angVelocity *= powf(damping, dt);
   }

   UpdateMatrix();

   float motion =  dot(m_linVelocity, m_linVelocity) + dot(m_angVelocity, m_angVelocity);

   float bias = 0.96f;
   m_rwaMotion = bias*m_rwaMotion + (1-bias)*motion;
   if (m_rwaMotion> 50.0f) m_rwaMotion = 5.0f;
}

void CCube::UpdatePosition( float dt )
{
   if( !m_awake )
   {
      return;
   }

   if( !m_bStatic )
   {
      m_c += (m_linVelocity * dt);

      vec4 angVel = m_angVelocity;

      quat Qvel = 0.5f * m_rot * quat(0, -angVel.x, -angVel.y, -angVel.z);
      m_rot = m_rot + (Qvel * dt);
      m_rot = normalize( m_rot );

      m_forces  = vec3( 0, 0, 0 );
      m_torques = vec4( 0, 0, 0, 1 );
   }

   UpdateMatrix();
}

void CCube::UpdateMatrix()
{
   mat4 matR = mat4_cast( m_rot );

   m_u[0] = vec4( 1, 0, 0, 1 );
   m_u[1] = vec4( 0, 1, 0, 1 );
   m_u[2] = vec4( 0, 0, 1, 1 );

   // TODO - LOOK HERE
   m_u[0] = m_u[0] * matR;
   m_u[1] = m_u[1] * matR;
   m_u[2] = m_u[2] * matR;

   mat4 matT;
   matT[0].w = m_c.x;
   matT[1].w = m_c.y;
   matT[2].w = m_c.z;
   m_matWorld = matR * matT;

   for( int i=0; i<NumVertices; i++ )
   {
      m_points[i] = m_origPoints[i] * m_matWorld;
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

void CCube::AddCollisionImpulse( CCube* cube1, CCube* cube2, vec3 &hitPoint, vec3 &normal, float dt, float penetration )
{
   // Some simple check code.
   if (dt <= 0.0){ return; }

   float invMass0 = (cube1->m_mass>1000.0f) ? 0.0f : (1.0f/cube1->m_mass);
   float invMass1 = (cube2->m_mass>1000.0f) ? 0.0f : (1.0f/cube2->m_mass);

   invMass0 = (!cube1->m_awake) ? 0.0f : invMass0;
   invMass1 = (!cube2->m_awake) ? 0.0f : invMass1;

   // Both objects are non movable
   if ( (invMass0+invMass1)==0.0 ) return;

   vec3 r0 = hitPoint - cube1->m_c;
   vec3 r1 = hitPoint - cube2->m_c;

   vec3 v0 = cube1->m_linVelocity + cross(vec3(cube1->m_angVelocity), r0);
   vec3 v1 = cube2->m_linVelocity + cross(vec3(cube2->m_angVelocity), r1);

   // Relative Velocity
   vec3 dv = v0 - v1;

   // NORMAL Impulse Code

   // Compute Normal Impulse
   float vn = dot(dv, normal);

   // Works out the bias to prevent Prevents sinking!
   const float allowedPenetration = 0.1f;
   const float biasFactor = 0.1f; // 0.1 to 0.3
   float biasFactorValue = g_positionCorrection ? biasFactor : 0.0f;

   float inv_dt = dt > 0.0f ? 1.0f / dt : 0.0f;
   float bias = biasFactorValue * inv_dt * glm::max(0.0f, penetration - allowedPenetration);

   vec3 crossR0Norm = cross(r0, normal);
   vec3 crossR1Norm = cross(r1, normal);
   float kNormal = invMass0 + invMass1+  

      dot( normal, 
      cross(vec3(vec4(crossR0Norm.x, crossR0Norm.y, crossR0Norm.z, 1) * cube1->m_invInertia), r0) +
      cross(vec3(vec4(crossR1Norm.x, crossR1Norm.y, crossR1Norm.z, 1) * cube2->m_invInertia), r1)
      );

   float massNormal = 1.0f / kNormal;

   float dPn = massNormal * ( -vn + bias );

   dPn = glm::max(dPn, 0.0f);

   // Apply normal contact impulse
   vec3 P = dPn * normal;
   vec3 crossR0P = cross(r0, P);
   vec3 crossR1P = cross(r1, P);
   cube1->m_linVelocity += invMass0 * P;
   cube1->m_angVelocity += vec4(crossR0P.x, crossR0P.y, crossR0P.z, 1) * cube1->m_invInertia;

   cube2->m_linVelocity -= invMass1 * P;
   cube2->m_angVelocity -= vec4(crossR1P.x, crossR1P.y, crossR1P.z, 1) * cube2->m_invInertia;
   // NORMAL

   // TANGENT Impulse Code
   {
      // Work out our tangent vector, with is perpendicular
      // to our collision normal
      // TODO - LOOK HERE
      vec3 tangent = vec3(0,0,0);
      tangent = dv - (dot(dv, normal) * normal);
      if( tangent.x != 0 || tangent.y != 0 || tangent.z != 0 )
      {
         tangent = normalize(tangent);
      }

      vec3 crossR0Tan = cross(r0, tangent);
      vec3 crossR1Tan = cross(r1, tangent);
      float kTangent = invMass0 + invMass1+  

         dot( tangent, 
         cross(vec3(vec4(crossR0Tan.x, crossR0Tan.y, crossR0Tan.z, 1) * cube1->m_invInertia), r0) +
         cross(vec3(vec4(crossR1Tan.x, crossR1Tan.y, crossR1Tan.z, 1) * cube2->m_invInertia), r1)
         );

      float massTangent = 1.0f / kTangent;

      float vt = dot(dv, tangent);
      float dPt = massTangent * (-vt);

      float maxPt = g_friction * dPn;
      dPt = CCollisionManager::Clamp(dPt, -maxPt, maxPt);

      // Apply contact impulse
      vec3 P = dPt * tangent;

      vec3 crossR0P2 = cross(r0, P);
      vec3 crossR1P2 = cross(r1, P);

      cube1->m_linVelocity += invMass0 * P;
      cube1->m_angVelocity += vec4(crossR0P2.x, crossR0P2.y, crossR0P2.z, 1) * cube1->m_invInertia;

      cube2->m_linVelocity -= invMass1 * P;
      cube2->m_angVelocity -= vec4(crossR1P2.x, crossR1P2.y, crossR1P2.z, 1) * cube2->m_invInertia;
   }
   // TANGENT
}