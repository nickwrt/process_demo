// process_demoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "process_demo.h"
#include "process_demoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcess_demoDlg dialog

CProcess_demoDlg::CProcess_demoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcess_demoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProcess_demoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProcess_demoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcess_demoDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProcess_demoDlg, CDialog)
	//{{AFX_MSG_MAP(CProcess_demoDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON2, OnButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON3, OnButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON4, OnButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON5, OnButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON6, OnButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON7, OnButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON8, OnButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON9, OnButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON10, OnButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON11, OnButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON12, OnButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON1, OnButtonUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcess_demoDlg message handlers

BOOL CProcess_demoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	InitButtonDisplay();
	InitCarInfo();
	LoadConfig();
	UpdateCarInfo();

	SetTimer(1001, 1000, NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CProcess_demoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProcess_demoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProcess_demoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CProcess_demoDlg::LoadConfig()
{
	int i,j;
	char buf[100] = {0};
	CString cs;

	m_gSystemInfo.carCacType = CAC_BY_CYCLE;

	i = GetPrivateProfileString("系统配置","赛道计量方式","计圈",buf,sizeof(buf),SYSTEM_PROFILE);
	if(i > 0 && 0 == strcmp("计时", buf))
	{
		m_gSystemInfo.carCacType = CAC_BY_TIME;
	}

	m_gSystemInfo.carMaxTime = MAX_TIME_CNT;
	i = GetPrivateProfileString("系统配置","计时最长时间","5",buf,sizeof(buf),SYSTEM_PROFILE);
	if(i > 0 && strlen(buf) > 0)
	{
		m_gSystemInfo.carMaxTime = (int)atol(buf) * 60;
	}

	m_gSystemInfo.carMaxCycle = MAX_CYCLE_CNT;
	i = GetPrivateProfileString("系统配置","计圈最大圈数","5",buf,sizeof(buf),SYSTEM_PROFILE);
	if(i > 0 && strlen(buf) > 0)
	{
		m_gSystemInfo.carMaxCycle = (int)atol(buf);
	}

	for(i=0; i<MAX_CAR_CNT; i++)
	{
		memset(buf, 0, sizeof(buf));
		m_gCarInfo[i].carName.Empty();
		m_gCarInfo[i].carSeqIDstr.Empty();
		m_gCarInfo[i].carCycleCnt = 0;
		m_gCarInfo[i].carSeqIDint = 0;
		m_gCarInfo[i].carTimeSec  = 0;

		cs.Format("%d号赛车", i+1);
		j = GetPrivateProfileString("车辆信息",cs,"",buf,sizeof(buf),SYSTEM_PROFILE);
		if(j > 0 && 0 != strlen(buf))
		{
			m_gCarInfo[i].carName.Format("%s", cs);
			m_gCarInfo[i].carSeqIDstr.Format("%s", buf);
			m_gCarInfo[i].carSeqIDint = (int)atol(buf);
			m_gCarInfo[i].carCurState = CAR_ST_INIT;
		}
	}
}

void CProcess_demoDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent == 1001)
	{
		/* 1秒钟刷新一次状态 */
		for(int i=0; i<MAX_CAR_CNT; i++)
		{
			if(!m_gCarInfo[i].carName.IsEmpty() && m_gCarInfo[i].carCurState > CAR_ST_INIT)
			{
				//m_gCarInfo[i].carCycleCnt ++;
				m_gCarInfo[i].carTimeSec ++;

				if(m_gSystemInfo.carCacType == CAC_BY_TIME && m_gCarInfo[i].carTimeSec >= m_gSystemInfo.carMaxTime)
				{
					m_Btn[i].SetUpColor(RGB(255,0,0));
					m_gCarInfo[i].carCurState = CAR_ST_OUTFLOW;
				}
			}
		}
		UpdateCarInfo();
	}

	CDialog::OnTimer(nIDEvent);
}

void CProcess_demoDlg::UpdateCarInfo()
{
	CString cs;

	for(int i=0; i<MAX_CAR_CNT; i++)
	{
		if(!m_gCarInfo[i].carName.IsEmpty())
		{
			this->GetDlgItem(IDC_CAR_LABEL1 + i)->SetWindowText(m_gCarInfo[i].carName);

			cs.Format("");
			if(m_gSystemInfo.carCacType == CAC_BY_CYCLE)
			{
				cs.Format("%d圈", m_gCarInfo[i].carCycleCnt);
			}
			else if(m_gSystemInfo.carCacType == CAC_BY_TIME)
			{
				cs.Format("%d分钟%d秒", (int)(m_gCarInfo[i].carTimeSec)/60, (int)m_gCarInfo[i].carTimeSec%60);
			}
			this->GetDlgItem(IDC_BUTTON1 + i)->SetWindowText(cs);

			this->GetDlgItem(IDC_CAR_LABEL1 + i)->ShowWindow(TRUE);
			this->GetDlgItem(IDC_BUTTON1 + i)->ShowWindow(TRUE);
		}
	}
}

void CProcess_demoDlg::InitCarInfo()
{
	for(int i=0; i<MAX_CAR_CNT; i++)
	{
		this->GetDlgItem(IDC_CAR_LABEL1 + i)->ShowWindow(FALSE);
		this->GetDlgItem(IDC_BUTTON1 + i)->ShowWindow(FALSE);
	}
}

void CProcess_demoDlg::OnButtonUpdate() 
{
	CString cs;

	int id = this->GetFocus()->GetDlgCtrlID() - IDC_BUTTON1;	

	if(id >= 0 && id < MAX_CAR_CNT)
	{
		m_gCarInfo[id].carCycleCnt = 0;
		m_gCarInfo[id].carTimeSec  = 0;

		m_gCarInfo[id].carCurState = CAR_ST_RUNNING;
		m_Btn[id].SetUpColor(RGB(0,0,255));

		cs.Format("");
		if(m_gSystemInfo.carCacType == CAC_BY_CYCLE)
		{
			cs.Format("%d圈", m_gCarInfo[id].carCycleCnt);
		}
		else if(m_gSystemInfo.carCacType == CAC_BY_TIME)
		{
			cs.Format("%d分钟%d秒", (int)(m_gCarInfo[id].carTimeSec)/60, (int)m_gCarInfo[id].carTimeSec%60);
		}
		this->GetDlgItem(IDC_BUTTON1 + id)->SetWindowText(cs);
	}
}

void CProcess_demoDlg::InitButtonDisplay()
{
	for(int i=0; i<MAX_CAR_CNT; i++)
	{
		this->GetDlgItem(IDC_BUTTON1 + i)->ModifyStyle(0,BS_OWNERDRAW,0);
		m_Btn[i].Attach(IDC_BUTTON1 + i, this);
		//m_Btn[i].SetDownColor(RGB(255,0,0));
		//m_Btn[i].SetUpColor(RGB(0,0,255));
	}
}
