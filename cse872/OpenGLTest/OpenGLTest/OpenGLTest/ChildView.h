#pragma once

#include "ShaderWnd\ShaderWnd.h"
#include "CollisionManager.h"

class CCube;

class CChildView : public CShaderWnd
{
public:
	CChildView();
   virtual ~CChildView();

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   virtual void InitGL();
   virtual void RenderGL();
   void Step( float dt );
   void AddCube( CCube* cube );
   void AddCube( vec3 pos, vec3 rot, vec3 size, float mass, BOOL bShowOneSide = FALSE, BOOL bStatic = FALSE );
   void UpdateSleepingObjects();
   void SetupScene( int sceneConfig );

   vector<CCube*> m_cubes;
   CCollisionManager m_colManager;

   bool m_bTimer;
   int m_nTimer;
   float m_fT;

   int m_sceneConfig;

protected:
	DECLARE_MESSAGE_MAP()
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnOperationTimer();
   afx_msg void OnTimer(UINT_PTR nIDEvent);
   afx_msg void OnOperationShootcube();
public:
   afx_msg void OnConfigurationsSmallPyramid();
   afx_msg void OnConfigurationsSingleCube();
   afx_msg void OnConfigurationsMediumPyramid();
   afx_msg void OnConfigurationsLargePyramid();
   afx_msg void OnConfigurationsTeeter();
};

