#if !defined(AFX_MYBUTTON_H__CFE89A1D_103B_44C4_A110_A433D451DD77__INCLUDED_)
#define AFX_MYBUTTON_H__CFE89A1D_103B_44C4_A110_A433D451DD77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyButton window

class CMyButton : public CButton
{
// Construction
public:
	CMyButton();

// Attributes
public:
	COLORREF m_TextColor, m_DownColor,m_UpColor;

// Operations
public:
	void SetDownColor(COLORREF color);
	void SetUpColor(COLORREF color);

	BOOL Attach(const UINT nID, CWnd* pParent);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyButton();

	// Generated message map functions
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//{{AFX_MSG(CMyButton)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYBUTTON_H__CFE89A1D_103B_44C4_A110_A433D451DD77__INCLUDED_)
