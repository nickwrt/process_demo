// process_demo.h : main header file for the PROCESS_DEMO application
//

#if !defined(AFX_PROCESS_DEMO_H__24A6F5E1_B46A_4C7E_A885_A115FAAC5514__INCLUDED_)
#define AFX_PROCESS_DEMO_H__24A6F5E1_B46A_4C7E_A885_A115FAAC5514__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CProcess_demoApp:
// See process_demo.cpp for the implementation of this class
//

class CProcess_demoApp : public CWinApp
{
public:
	CProcess_demoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcess_demoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CProcess_demoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESS_DEMO_H__24A6F5E1_B46A_4C7E_A885_A115FAAC5514__INCLUDED_)
