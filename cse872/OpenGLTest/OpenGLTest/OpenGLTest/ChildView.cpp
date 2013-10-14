
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
   m_sceneConfig = 1;
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
   ON_WM_KEYUP()
   ON_WM_TIMER()
   ON_COMMAND(ID_OPERATION_TIMER, &CChildView::OnOperationTimer)
   ON_COMMAND(ID_OPERATION_SHOOTCUBE, &CChildView::OnOperationShootcube)
   ON_COMMAND(ID_CONFIGURATIONS_SMALLPYRAMID, &CChildView::OnConfigurationsSmallPyramid)
   ON_COMMAND(ID_CONFIGURATIONS_SINGLECUBE, &CChildView::OnConfigurationsSingleCube)
   ON_COMMAND(ID_CONFIGURATIONS_MEDIUMPYRAMID, &CChildView::OnConfigurationsMediumPyramid)
   ON_COMMAND(ID_CONFIGURATIONS_LARGEPYRAMID, &CChildView::OnConfigurationsLargePyramid)
   ON_COMMAND(ID_CONFIGURATIONS_TEETER, &CChildView::OnConfigurationsTeeter)
   ON_COMMAND(ID_OPERATION_RESET, &CChildView::OnOperationReset)
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
   m_vCenter.z -= 60;

   SetupScene( 1 );

   m_program = LoadShaders( "ShaderWnd/vertex.glsl", "ShaderWnd/fragment.glsl" );

   m_nPVM = glGetUniformLocation(m_program, "mPVM");

   glClearColor( 0.0, 0.0, 0.0, 1.0 );
   glEnable( GL_DEPTH_TEST );

   OnOperationTimer();
}

void CChildView::SetupScene( int sceneConfig )
{
   m_sceneConfig = sceneConfig;
   m_cubes.clear();
   m_colManager.Clear();

   switch( m_sceneConfig )
   {
   case 1:
      {
         AddCube( vec3( 0, 5, 10 ), vec3( 0, 0, 0 ), vec3( 2, 2, 2 ), 5.0f );

         AddCube( vec3( -4, 5, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );
         AddCube( vec3( 0, 5, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );
         AddCube( vec3( 4, 5, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );

         AddCube( vec3( -2, 7, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );
         AddCube( vec3( 2, 7, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );

         AddCube( vec3( 0, 9, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );

         break;
      }
   case 2:
      {
         AddCube( vec3( 0, 5, 0 ), vec3( 0, 0, 0 ), vec3( 2, 2, 2 ), 5.0f );

         break;
      }
   case 3:
      {
         AddCube( vec3( 0, 5, 10 ), vec3( 0, 0, 0 ), vec3( 2, 2, 2 ), 5.0f );

         AddCube( vec3( -6, 5, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );
         AddCube( vec3( -2, 5, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );
         AddCube( vec3( 2, 5, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );
         AddCube( vec3( 6, 5, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );

         AddCube( vec3( -4, 7, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );
         AddCube( vec3( 0, 7, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );
         AddCube( vec3( 4, 7, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );

         AddCube( vec3( -2, 9, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );
         AddCube( vec3( 2, 9, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );

         AddCube( vec3( 0, 11, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );

         break;
      }
   case 4:
      {
         AddCube( vec3( 0, 5, 10 ), vec3( 0, 0, 0 ), vec3( 2, 2, 2 ), 5.0f );

         AddCube( vec3( -8, 5, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );
         AddCube( vec3( -4, 5, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );
         AddCube( vec3( 0, 5, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );
         AddCube( vec3( 4, 5, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );
         AddCube( vec3( 8, 5, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );

         AddCube( vec3( -6, 7, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );
         AddCube( vec3( -2, 7, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );
         AddCube( vec3( 2, 7, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );
         AddCube( vec3( 6, 7, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );

         AddCube( vec3( -4, 9, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );
         AddCube( vec3( 0, 9, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );
         AddCube( vec3( 4, 9, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );

         AddCube( vec3( -2, 11, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );
         AddCube( vec3( 2, 11, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );

         AddCube( vec3( 0, 13, 0 ), vec3( 0, M_PI_4, 0 ), vec3( 2, 2, 2 ), 5.0f );

         break;
      }
   case 5:
      {
         AddCube( vec3( 0, 5, -10 ), vec3( 0, 0, 0 ), vec3( 2, 2, 2 ), 5.0f );

         AddCube( vec3( 0, 2, 0 ), vec3( 0, 0, 0 ), vec3( .5, 2.5, 5 ), 1000, FALSE, FALSE ); // base
         AddCube( vec3( -.5, 4, 0 ), vec3( 0, 0, -.25 ), vec3( 15, .25, 3 ), 10.0f ); // plank
         AddCube( vec3( 6, 15, 0 ), vec3( 0, 0, 0 ), vec3( 2, 2, 2 ), 30.0f ); // launcher
         AddCube( vec3( -6, 7, 0 ), vec3( 0, 0, 0 ), vec3( 1, 1, 1 ), 1.0f ); // launched

         break;
      }
   default:
      {
         break;
      }
   }

   AddCube( vec3( 0, -2, 0 ), vec3( 0, 0, 0 ), vec3( 30, 5, 30 ), 100000, TRUE, TRUE );
   AddCube( vec3( -17.5, 15.5, 0 ), vec3( 0, 0, M_PI_2 ), vec3( 30, 5, 30 ), 100000, TRUE, TRUE );
   AddCube( vec3( 0, 15.5, -17.5 ), vec3( 0, M_PI_2, M_PI_2 ), vec3( 30, 5, 30 ), 100000, TRUE, TRUE );
   AddCube( vec3( 17.5, 15.5, 0 ), vec3( 0, 0, -M_PI_2 ), vec3( 30, 5, 30 ), 100000, TRUE, TRUE );
   AddCube( vec3( 0, 15.5, 17.5 ), vec3( 0, -M_PI_2, M_PI_2 ), vec3( 30, 5, 30 ), 100000, TRUE, TRUE );
   AddCube( vec3( 0, 33, 0 ), vec3( M_PI, M_PI, 0 ), vec3( 30, 5, 30 ), 100000, TRUE, TRUE );

   for( int i=0; i<m_cubes.size(); i++ )
   {
      m_cubes.at( i )->InitGL();
   }
}

void CChildView::AddCube( vec3 pos, vec3 rot, vec3 size, float mass, BOOL bShowOneSide, BOOL bStatic )
{
   auto pCube = new CCube( pos, rot, size, mass, bShowOneSide, bStatic );
   AddCube( pCube );
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

   //UpdateSleepingObjects();

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
      {
         SetupScene( m_sceneConfig );
         break;
      }
   case 'z':
   case 'Z':
      {
         OnOperationShootcube();
         break;
      }
   case '1':
      {
         SetupScene( 1 );
         break;
      }
   case '2':
      {
         SetupScene( 2 );
         break;
      }
   case '3':
      {
         SetupScene( 3 );
         break;
      }
   case '4':
      {
         SetupScene( 4 );
         break;
      }
   case '5':
      {
         SetupScene( 5 );
         break;
      }
   case VK_UP:
   case VK_DOWN:
   case VK_LEFT:
   case VK_RIGHT:
   case VK_SPACE:
   case 'C':
   case 'c':
   case 'q':
   case 'Q':
   case 'w':
   case 'W':
   case 'e':
   case 'E':
   case 'a':
   case 'A':
   case 's':
   case 'S':
   case 'd':
   case 'D':
      {
         m_keysPressed[nChar] = TRUE;
      }
   }

   CShaderWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CChildView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   switch(nChar)
   {
   case VK_UP:
   case VK_DOWN:
   case VK_LEFT:
   case VK_RIGHT:
   case VK_SPACE:
   case 'C':
   case 'c':
   case 'q':
   case 'Q':
   case 'w':
   case 'W':
   case 'e':
   case 'E':
   case 'a':
   case 'A':
   case 's':
   case 'S':
   case 'd':
   case 'D':
      {
         m_keysPressed[nChar] = FALSE;
      }
   }

   CShaderWnd::OnKeyUp(nChar, nRepCnt, nFlags);
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
      m_nTimer = SetTimer(1, 20, NULL);
   }
}

void CChildView::OnTimer(UINT_PTR nIDEvent)
{
   m_fT = 0.02f;
   InvalidateRect(NULL,FALSE);
   float angSpeed = 40.0f;
   float linSpeed = 30.0f;

   if( m_keysPressed[VK_UP] )
   {
      m_cubes[0]->m_linVelocity.z -= linSpeed * m_fT;
   }
   if( m_keysPressed[VK_DOWN] )
   {
      m_cubes[0]->m_linVelocity.z += linSpeed * m_fT;
   }
   if( m_keysPressed[VK_LEFT] )
   {
      m_cubes[0]->m_linVelocity.x -= linSpeed * m_fT;
   }
   if( m_keysPressed[VK_RIGHT] )
   {
      m_cubes[0]->m_linVelocity.x += linSpeed * m_fT;
   }
   if( m_keysPressed[VK_SPACE] )
   {
      m_cubes[0]->m_linVelocity.y += linSpeed * m_fT;
   }
   if( m_keysPressed['c'] || m_keysPressed['C'] )
   {
      m_cubes[0]->m_linVelocity.y -= linSpeed * m_fT;
   }
   if( m_keysPressed['q'] || m_keysPressed['Q'] )
   {
      m_cubes[0]->m_angVelocity.x += angSpeed * m_fT;
   }
   if( m_keysPressed['w'] || m_keysPressed['W'] )
   {
      m_cubes[0]->m_angVelocity.y += angSpeed * m_fT;
   }
   if( m_keysPressed['e'] || m_keysPressed['E'] )
   {
      m_cubes[0]->m_angVelocity.z += angSpeed * m_fT;
   }
   if( m_keysPressed['a'] || m_keysPressed['A'] )
   {
      m_cubes[0]->m_angVelocity.x -= angSpeed * m_fT;
   }
   if( m_keysPressed['s'] || m_keysPressed['S'] )
   {
      m_cubes[0]->m_angVelocity.y -= angSpeed * m_fT;
   }
   if( m_keysPressed['d'] || m_keysPressed['D'] )
   {
      m_cubes[0]->m_angVelocity.z -= angSpeed * m_fT;
   }

   CShaderWnd::OnTimer(nIDEvent);
}

void CChildView::AddCube( CCube* cube )
{
   m_cubes.push_back( cube );
   m_colManager.AddCube( cube );
}

void CChildView::OnOperationShootcube()
{
   CCube* cube = new CCube( vec3( 0, 15, 0 ), vec3( M_PI_4, M_PI_4, M_PI_4 ), vec3( 2, 2, 2 ), 5.0f );
   //m_colManager.m_collisionsArray.clear();
   AddCube(cube);
   cube->InitGL();
}

void CChildView::OnConfigurationsSmallPyramid()
{
   SetupScene( 1 );
}

void CChildView::OnConfigurationsSingleCube()
{
   SetupScene( 2 );
}

void CChildView::OnConfigurationsMediumPyramid()
{
   SetupScene( 3 );
}

void CChildView::OnConfigurationsLargePyramid()
{
   SetupScene( 4 );
}

void CChildView::OnConfigurationsTeeter()
{
   SetupScene( 5 );
}

void CChildView::OnOperationReset()
{
   SetupScene( m_sceneConfig );
}
