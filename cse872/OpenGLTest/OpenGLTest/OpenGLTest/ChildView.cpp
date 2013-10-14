
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "OpenGLTest.h"
#include "ChildView.h"
#include "Helpers.h"
#include "Cube.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CChildView::CChildView()
{
   m_bTimer = false;
   m_nTimer = 0;
   m_fT = 0.f;
}

CChildView::~CChildView()
{
   for( int i=0; i<m_cubes.size(); i++ )
   {
      delete m_cubes.at( i );
   }

   m_cubes.clear();
}

BEGIN_MESSAGE_MAP(CChildView, CShaderWnd)
   ON_WM_PAINT()
   ON_WM_KEYDOWN()
   ON_WM_TIMER()
   ON_COMMAND(ID_OPERATION_TIMER, &CChildView::OnOperationTimer)
END_MESSAGE_MAP()

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
   if (!CShaderWnd::PreCreateWindow(cs))
      return FALSE;

   cs.dwExStyle |= WS_EX_CLIENTEDGE;
   cs.style &= ~WS_BORDER;
   cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
      ::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

   return TRUE;
}

void CChildView::InitGL()
{
   CCube* cube = new CCube( vec3( 0, 2, 0 ), vec3( 2, 2, 2 ), vec3( 2, 2, 2 ), 1.0f );
   CCube* bottomCube = new CCube( vec3( 0, -2, 0 ), vec3( 0, 0, 0 ), vec3( 10, 1, 10 ), 100000, TRUE );
   AddCube( cube );
   AddCube( bottomCube );

   m_program = LoadShaders( "ShaderWnd/vertex.glsl", "ShaderWnd/fragment.glsl" );

   for( int i=0; i<m_cubes.size(); i++ )
   {
      m_cubes.at( i )->InitGL();
   }

   m_nPVM = glGetUniformLocation(m_program, "mPVM");

   glClearColor( 0.0, 0.0, 0.0, 1.0 );
   glEnable( GL_DEPTH_TEST );
}

void CChildView::RenderGL()
{
   Step( m_fT );

   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   glUniformMatrix4fv(m_nPVM, 1, GL_FALSE, value_ptr(m_mPVM));

   glUniform1f(glGetUniformLocation(m_program,"t"), m_fT);

   for( int i=0; i<m_cubes.size(); i++ )
   {
      m_cubes.at( i )->RenderGL( m_program );
   }
}

void CChildView::UpdateSleepingObjects()
{
   //return;
   // For an object to remain sleeping, it must be in collision with another
   // sleeping object or infinite mass object
   // or its energy/motion is less than a certain threshold

   for (int k=0; k<m_colManager.m_collisionsArray.size(); k++)
   {	
      // Check its hitting another object
      bool sleepingCollision = false;
      for (int i=0; i<m_colManager.m_collisionsArray.size(); i++)
      {
         stCollisionPoints* cp = &m_colManager.m_collisionsArray[ i ];

         CCube* box0				= cp->box0;
         CCube* box1				= cp->box1;

         CCube* sleepCube = m_cubes[k];
         CCube* otherCube = NULL;

         if (sleepCube == box0) otherCube=box1;
         if (sleepCube == box1) otherCube=box0;

         if (otherCube)
         {
            if (!otherCube->m_awake || otherCube->m_mass>1000.0f)
            {
               sleepingCollision |= true;
            }
         }
      }

      if (!sleepingCollision)
      {
         m_cubes[k]->m_rwaMotion = 2 * g_sleepEpsilon;
         m_cubes[k]->m_awake = true;
      }

      // Check its energy/motion
      if (m_cubes[k]->m_rwaMotion < g_sleepEpsilon)
      {
         m_cubes[k]->m_awake = false;
         m_cubes[k]->m_linVelocity = vec3(0,0,0);
         m_cubes[k]->m_angVelocity = vec4(0,0,0,1);
      }
      else if (m_cubes[k]->m_rwaMotion > 10 * g_sleepEpsilon)
      {
         m_cubes[k]->m_rwaMotion = 10 * g_sleepEpsilon;
         m_cubes[k]->m_awake = true;
      }

      // Check if a cube in collision with our sleeping cube
      // has enough energy to wake it up
      for (int i=0; i<m_colManager.m_collisionsArray.size(); i++)
      {
         stCollisionPoints* cp = &m_colManager.m_collisionsArray[ i ];

         CCube* box0				= cp->box0;
         CCube* box1				= cp->box1;

         CCube* sleepCube = m_cubes[k];
         CCube* otherCube = NULL;

         if (sleepCube == box0) otherCube=box1;
         if (sleepCube == box1) otherCube=box0;

         if (otherCube && otherCube->m_mass<1000.0f)
         {
            if (otherCube->m_rwaMotion > (2 * g_sleepEpsilon))
            {
               m_cubes[k]->m_awake = true;
               m_cubes[k]->m_rwaMotion = 2 * g_sleepEpsilon;
            }
         }
      }
   }
}

int g_numIterations = 7;

void CChildView::Step( float dt )
{
   m_colManager.DetectCollisions();

   UpdateSleepingObjects();

   for( int i=0; i<m_cubes.size(); i++ )
   {
      m_cubes.at( i )->UpdateVelocity( dt );
   }

   for (int i=0; i<g_numIterations; i++)
   {
      m_colManager.ApplyImpulses(dt);
   }

   for( int i=0; i<m_cubes.size(); i++ )
   {
      m_cubes.at( i )->UpdatePosition( dt );
   }
}

void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   switch(nChar)
   {
   case 'r':
   case 'R':
      //ResetMatrix();
      break;
   }

   CShaderWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CChildView::OnOperationTimer()
{
   if (m_bTimer)
   {
      m_bTimer = false;
      if (m_nTimer)
         KillTimer(m_nTimer);
   } else
   {
      m_bTimer = true;
      m_fT = 0.f;
      m_nTimer = SetTimer(1, 10, NULL);
   }
}

void CChildView::OnTimer(UINT_PTR nIDEvent)
{
   m_fT = 0.01f;
   InvalidateRect(NULL,FALSE);

   CShaderWnd::OnTimer(nIDEvent);
}

void CChildView::AddCube( CCube* cube )
{
   m_cubes.push_back( cube );
   m_colManager.AddCube( cube );
}