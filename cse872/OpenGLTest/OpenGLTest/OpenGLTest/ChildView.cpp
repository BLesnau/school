
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
   delete m_cube;
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
   m_cube = new CCube();

   m_program = LoadShaders( "ShaderWnd/vertex.glsl", "ShaderWnd/fragment.glsl" );

   m_cube->InitGL();

   /* GLuint vao;
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);

   GLuint buffer;
   glGenBuffers( 1, &buffer );
   glBindBuffer( GL_ARRAY_BUFFER, buffer );
   glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW );
   glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
   glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );

   GLuint vPosition = glGetAttribLocation( m_program, "vPosition" );
   glEnableVertexAttribArray( vPosition );
   glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
   GLuint vColor = glGetAttribLocation( m_program, "vColor" );
   glEnableVertexAttribArray( vColor );
   glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)) );*/

   //**************************************
   /*glBufferData(GL_ARRAY_BUFFER, sizeof(points) +           sizeof(normals), NULL, GL_STATIC_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER, 0,                sizeof(normals), points);
   glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(normals), normals);

   point4 light_position (-5.f, 5.f, -5.f, 0.f);
   color4 light_ambient (0.2f, 0.2f, 0.2f, 1.f);
   color4 light_diffuse (1.f, 1.f, 1.f, 1.f);
   color4 light_specular (1.f, 1.f, 1.f, 1.f);
   color4 material_ambient(.3f, .6f, .3f, 1.f);
   color4 material_diffuse (0.3f, .6f, 0.3f, 1.f);
   color4 material_specular (1.f, 1.f, 1.f, 1.f);
   float material_shininess = 100.0f;
   color4 ambient_product = light_ambient*material_ambient;
   color4 diffuse_product = light_diffuse*material_diffuse;
   color4 specular_product = light_specular*material_specular;*/
//**************************************

   m_nPVM = glGetUniformLocation(m_program, "mPVM");

   glClearColor( 0.0, 0.0, 0.0, 1.0 );
   glEnable( GL_DEPTH_TEST );
}

void CChildView::RenderGL()
{
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   glUniformMatrix4fv(m_nPVM, 1, GL_FALSE, value_ptr(m_mPVM));
   //glUniformMatrix4fv(m_nVM, 1, GL_FALSE, value_ptr(m_mVM));

   glUniform1f(glGetUniformLocation(m_program,"t"), m_fT);

   m_cube->RenderGL( m_program );
   //glDrawArrays( GL_TRIANGLES, 0, NumVertices );

   //SwapBuffers( *(this->GetWindowDC()) );
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
      m_nTimer = SetTimer(1, 40, NULL);
   }
}

void CChildView::OnTimer(UINT_PTR nIDEvent)
{
   m_fT += 0.04f;
   InvalidateRect(NULL,FALSE);

   CShaderWnd::OnTimer(nIDEvent);
}