
// DMProg.h : main header file for the DMProg application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CDMProgApp:
// See DMProg.cpp for the implementation of this class
//

class CDMProgApp : public CWinApp
{
public:
	CDMProgApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	
	DECLARE_MESSAGE_MAP()
};

extern CDMProgApp theApp;
