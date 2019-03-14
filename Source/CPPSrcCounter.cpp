// CPPSrcCounter.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "CPPSrcCounter.h"
#include "CPPSrcCounterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCPPSrcCounterApp

BEGIN_MESSAGE_MAP(CCPPSrcCounterApp, CBCGPWinApp)
	ON_COMMAND(ID_HELP, CBCGPWinApp::OnHelp)
END_MESSAGE_MAP()


// CCPPSrcCounterApp construction

CCPPSrcCounterApp::CCPPSrcCounterApp()
{
	// Enable Office 2010 look (Blue theme):
	//SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2010_BLUE);
	SetVisualTheme(BCGP_VISUAL_THEME_DEFAULT);

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CCPPSrcCounterApp object

CCPPSrcCounterApp theApp;


// CCPPSrcCounterApp initialization

BOOL CCPPSrcCounterApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CBCGPWinApp::InitInstance();

	CCPPSrcCounterDlg dlg;
	m_pMainWnd = &dlg;

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CCPPSrcCounterApp::ExitInstance()
{
	return CBCGPWinApp::ExitInstance();
}

