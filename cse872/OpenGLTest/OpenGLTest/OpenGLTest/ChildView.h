#pragma once

#include "ShaderWnd\ShaderWnd.h"

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

   CCube* m_cube;

   bool m_bTimer;
   int m_nTimer;
   float m_fT;

protected:
	DECLARE_MESSAGE_MAP()
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnOperationTimer();
   afx_msg void OnTimer(UINT_PTR nIDEvent);
};

