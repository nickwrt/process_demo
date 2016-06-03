// process_demoDlg.h : header file
//

#if !defined(AFX_PROCESS_DEMODLG_H__1AFC721F_6240_4402_8855_22818AFD4FE5__INCLUDED_)
#define AFX_PROCESS_DEMODLG_H__1AFC721F_6240_4402_8855_22818AFD4FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SystemDef.h"
#include "MyButton.h"

/////////////////////////////////////////////////////////////////////////////
// CProcess_demoDlg dialog

class CProcess_demoDlg : public CDialog
{
// Construction
public:
	void InitButtonDisplay();
	void InitCarInfo();
	void UpdateCarInfo();
	void LoadConfig();
	CProcess_demoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CProcess_demoDlg)
	enum { IDD = IDD_PROCESS_DEMO_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcess_demoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	CMyButton   m_Btn[MAX_CAR_CNT];
	CarInfo		m_gCarInfo[MAX_CAR_CNT];
	SystemDef	m_gSystemInfo;
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CProcess_demoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESS_DEMODLG_H__1AFC721F_6240_4402_8855_22818AFD4FE5__INCLUDED_)
