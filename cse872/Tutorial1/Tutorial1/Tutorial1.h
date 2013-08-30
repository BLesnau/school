
// Tutorial1.h : main header file for the Tutorial1 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CTutorial1App:
// See Tutorial1.cpp for the implementation of this class
//

class CTutorial1App : public CWinAppEx
{
public:
	CTutorial1App();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CTutorial1App theApp;
