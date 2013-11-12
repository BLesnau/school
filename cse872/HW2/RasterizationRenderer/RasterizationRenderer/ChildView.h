
// ChildView.h : interface of the CChildView class
//


#pragma once
#include "renderer/Gz.h"

// CChildView window

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:
	int m_nXres;
	int m_nYres;
	GzDisplay *m_Display;
	GzRender *m_Render;

// Operations
public:
	void Resize(int x, int y);

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTestsApp1();
	afx_msg void OnTestsApp2();
	afx_msg void OnTestsApp3();
};

