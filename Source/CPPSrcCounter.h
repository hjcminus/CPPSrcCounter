// CPPSrcCounter.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CCPPSrcCounterApp:
// See CPPSrcCounter.cpp for the implementation of this class
//

class CCPPSrcCounterApp :	public BASE_APP
{
public:

	CCPPSrcCounterApp();

	virtual BOOL InitInstance();
	virtual int  ExitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CCPPSrcCounterApp theApp;
