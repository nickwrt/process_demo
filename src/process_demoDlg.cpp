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

#define BAUDRATE_4800     1
#define BAUDRATE_9600     2
#define BAUDRATE_38400    3
#define BAUDRATE_57600    4
#define BAUDRATE_115200   5
#define BAUDRATE_230400   6

typedef struct RfidDev
{
	unsigned long rfid;
	
	unsigned long curtick;
	unsigned long lasttick;

	int losspower;
}RfidDev;

extern BOOL openCom(int com, int baudrate);
extern void closeCom(void);
extern void comThreadCreate(void);
extern void comRead(unsigned char * data, unsigned long len);
extern void comWrite(unsigned char * data, unsigned long len);

RfidDev	m_RfidInfo[MAX_CAR_CNT];

HANDLE g_hWinCom = INVALID_HANDLE_VALUE;  //全局变量，串口句柄
HANDLE g_comMutex = NULL; //互斥

#define COM_READ_BUF_SIZE  1024

unsigned char g_comReadBuf[COM_READ_BUF_SIZE];
unsigned long g_comReadBufLen = 0;

BOOL openCom(int com, int baudrate)
{
    char comStr[20];
    
    unsigned long rate;
    //bool ret;
    
    if (g_hWinCom != INVALID_HANDLE_VALUE)
    {
        closeCom();
    }
    
    WaitForSingleObject(g_comMutex, INFINITE);
    
    sprintf(comStr, "\\\\.\\COM%d", com);
    
    g_hWinCom = CreateFile(comStr, GENERIC_READ|GENERIC_WRITE,
		            0,NULL,OPEN_EXISTING,
				    FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
				    NULL);

    if (g_hWinCom == INVALID_HANDLE_VALUE)
    {
        ReleaseMutex(g_comMutex);
        return FALSE;
    }
 
        // Set the event mask.
    //SetCommMask(g_hWinCom, EV_RXCHAR/* | EV_RX80FULL | EV_TXEMPTY*/);

    SetupComm(g_hWinCom, 2048, 2048);   //设置输入输出缓存区

    COMMTIMEOUTS TimeOuts;

    //设定读超时
    TimeOuts.ReadIntervalTimeout=1000;
    TimeOuts.ReadTotalTimeoutMultiplier=0;
    TimeOuts.ReadTotalTimeoutConstant=0;
    //在读一次输入缓冲区的内容后读操作就立即返回，
    //而不管是否读入了要求的字符。

    //设定写超时
    TimeOuts.WriteTotalTimeoutMultiplier=100;
    TimeOuts.WriteTotalTimeoutConstant=500;

    SetCommTimeouts(g_hWinCom, &TimeOuts); //设置超时
    
    switch(baudrate)
    {
        case BAUDRATE_4800:
            rate = 4800;
            break;

        case BAUDRATE_9600:
            rate = 9600;
            break;

        case BAUDRATE_38400:
            rate = 38400;
            break;

        case BAUDRATE_57600:
            rate = 57600;
            break;

        case BAUDRATE_115200:
            rate = 115200;
            break;

        case BAUDRATE_230400:
            rate = 230400;
            break;

        default:
            rate = 9600;
            break;
        
    }

    DCB dcb;

    GetCommState(g_hWinCom, &dcb);

    dcb.BaudRate=rate; //波特率为9600

    dcb.ByteSize=8; //每个字节有8位

    dcb.Parity=NOPARITY; //无奇偶校验位

    dcb.StopBits=ONESTOPBIT; //1个停止位

    SetCommState(g_hWinCom, &dcb);

    PurgeComm(g_hWinCom, PURGE_TXCLEAR|PURGE_RXCLEAR);
    
    g_comReadBufLen = 0;
    
    ReleaseMutex(g_comMutex);
    
    return TRUE;
}

void comWrite(unsigned char * data, unsigned long len)
{
        //异步写串口	
	COMSTAT ComStat;
	DWORD  dwErrorFlags;
	BOOL   bWriteStat;

    OVERLAPPED osWrite; 
    memset(&osWrite,0,sizeof(OVERLAPPED)); 
    osWrite.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

	if(INVALID_HANDLE_VALUE == g_hWinCom)
	{
	    return;
	}
	
	WaitForSingleObject(g_comMutex, INFINITE);
	
	ClearCommError(g_hWinCom, &dwErrorFlags,&ComStat);	

    bWriteStat=WriteFile(g_hWinCom, data, len, &len, &osWrite);
    	
	if(!bWriteStat) 
    { 
        if(GetLastError()==ERROR_IO_PENDING) 
        { 
            WaitForSingleObject(osWrite.hEvent, 1000); 
            //GetOverlappedResult(g_hCom, &osWrite, &dwErrorFlags, TRUE );
        } 
    }
	
	ReleaseMutex(g_comMutex);
    //PurgeComm(g_hCom, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);	
}

void rfidRcvMsg(unsigned char msg[])
{
    unsigned long id;
    unsigned char lossPower;
    int i;
    
    if ((msg[0] ^ msg[1] ^ msg[2] ^ msg[3] ^ msg[4] ^ msg[5]) != msg[6])
    {
        //shellPrint("check error ");
        return;
    }

    id = (unsigned long)msg[5] 
      | ((unsigned long)msg[4] << 8)
      | ((unsigned long)msg[3] << 16)
      | ((unsigned long)msg[2] << 24);

    //lossPower = msg[1] & 0x80 ?  1 : 0;
	
	for(i=0; i<MAX_CAR_CNT; i++)
	{
		if(m_RfidInfo[i].rfid == id)
		{
			m_RfidInfo[i].curtick = GetTickCount();
			m_RfidInfo[i].losspower = lossPower = msg[1] & 0x80 ?  1 : 0;
			break;
		}
	}
	/*
	for (i = 0; i < g_labelNum; i++)
    {
        if (g_label[i].id == id)
        {
            g_label[i].lossPower = lossPower;
            g_label[i].msgCount[9]++;
                        
            return;
        }
    }
    
    //filtration count
    for(i=0; i<g_filtrationArrayNumber; i++)
	{		
	    if(g_filtrationArray[i].id == id)
	    {
			g_filtrationArray[i].count++;
			g_filtrationArray[i].lossPower = lossPower;
		
			if(g_filtrationArray[i].timeCount < FILTRATION_TIME)
			{
				if(g_filtrationArray[i].count >= FILTRATION_PAGE_COUNT)
				{
//					shellPrint("11timeCount = %d count = %d\r\n", g_filtrationArray[i].timeCount, g_filtrationArray[i].count);
					g_filtrationArray[i].count = 1;
					g_filtrationArray[i].timeCount = 0;
					
					addInfoToShowArray(g_filtrationArray[i].id, g_filtrationArray[i].lossPower);
				}
			}
			
			return;
		}
	}
		
	//add to filtration
	if(g_filtrationArrayNumber < TALBE_NUM)
	{
		g_filtrationArray[g_filtrationArrayNumber].id =id;
		g_filtrationArray[g_filtrationArrayNumber].count = 1;
		g_filtrationArray[g_filtrationArrayNumber].lossPower = lossPower;
		
		g_filtrationArrayNumber++;			
	}
		
//    if (g_labelNum < TALBE_NUM)
//    {
//        g_label[g_labelNum].id = id;
//        g_label[g_labelNum].lossPower = lossPower;
//     	    
//        for (i = 0;  i < 10; i++)
//        {
//            g_label[g_labelNum].msgCount[i] = 2;
//        }
//        
//        g_label[g_labelNum].keepSecond = 0;

//        g_labelNum++;
//    }
	*/
}

void comRead(unsigned char * data, unsigned long len)
{
    static unsigned char msg[7];
    static unsigned char msgLen = 0;
    int i;
    
    for (i = 0; i < (int)len; i++)
    {
        if (msgLen < 5)
        {
            if (data[i] != 0xFF)
            {
                msgLen = 0;
                //shellPrint("NOT FF %d ", i);
                continue;
            }
            else
            {
                msgLen++;
            }
        }
        else
        {
            msg[msgLen-5] = data[i];
            msgLen++;
            
            if (msgLen >= 12)
            {
                rfidRcvMsg(msg);
                
                msgLen = 0;
            }
        }
    }
}

void closeCom()
{
    if (g_hWinCom == INVALID_HANDLE_VALUE)
    {
        return;
    }

    WaitForSingleObject(g_comMutex, INFINITE);

    CloseHandle(g_hWinCom);
 
    g_hWinCom = INVALID_HANDLE_VALUE;

    ReleaseMutex(g_comMutex);
}

DWORD WINAPI ThreadWinCom(LPVOID pram)
{
    OVERLAPPED os; 

    //DWORD dwMask,dwTrans, dwError;

    //COMSTAT comStat;

    BOOL bReadStat;

    HANDLE event;
	COMSTAT ComStat;
	DWORD  dwErrorFlags;

    unsigned long len, rdLen;

    event = CreateEvent(NULL,TRUE,FALSE,NULL);

    memset(&os,0,sizeof(OVERLAPPED));    
    os.hEvent= event;

    g_comMutex = CreateMutex(NULL,FALSE,NULL); 
    
    g_comReadBufLen = 0;

    while(1)
    {
        if (g_hWinCom == INVALID_HANDLE_VALUE)
        {
            Sleep(1);
            continue;
        }

        //memset(&os,0,sizeof(OVERLAPPED));    
        //os.hEvent= event;
        WaitForSingleObject(g_comMutex, INFINITE);
        rdLen = 0;
        
        //len = 1024;

        ClearCommError(g_hWinCom, &dwErrorFlags, &ComStat);
        
        if (!ComStat.cbInQue)
        {
           ReleaseMutex(g_comMutex);
           Sleep(1);
           continue;
        }

        len = (ComStat.cbInQue >= COM_READ_BUF_SIZE) ? COM_READ_BUF_SIZE : ComStat.cbInQue;
/*        
        if(!WaitCommEvent(g_hWinCom,&rdLen,&os))
        {
            if(GetLastError() != ERROR_IO_PENDING )
            {
                //ReleaseMutex(g_comMutex);
                ClearCommError(g_hWinCom, &dwErrorFlags, &ComStat);
                Sleep(1);
                continue;
            }
        }
        
        ClearCommError(g_hWinCom, &dwErrorFlags, &ComStat);
*/
        bReadStat = ReadFile(g_hWinCom, g_comReadBuf, len, &rdLen, &os);
        
        
        if (!bReadStat)
        {
            if(GetLastError() != ERROR_IO_PENDING )
            {
                //ReleaseMutex(g_comMutex);
                ReleaseMutex(g_comMutex);
                //shellPrint("[E] ");
                Sleep(1);
                continue;
            }
            
            //WaitForSingleObject(event, INFINITE);
            
            //PurgeComm(g_hWinCom, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
            //PurgeComm(g_hWinCom, PURGE_RXCLEAR);
             
            GetOverlappedResult(g_hWinCom,&os, &rdLen, TRUE);
            //GetOverlappedResult(g_hCom, &os, &dwTrans, TRUE );
        }
        else
        {
             //;
        }
        
        //
        //PurgeComm(g_hWinCom, PURGE_RXCLEAR);
        //PurgeComm(g_hWinCom, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

        //g_comReadBufLen += rdLen;
        
        if (rdLen)
        {
            //WDM_read(g_comReadBuf, g_comReadBufLen);
            comRead(g_comReadBuf, rdLen); 
            g_comReadBufLen = 0;
        }
        
        ReleaseMutex(g_comMutex);

    }
}

void comThreadCreate(void)
{
    CreateThread(NULL, 0, ThreadWinCom, 0, 0, 0);
}

// 检查有哪几个串口可用
// 返回：32BIT，BIT0——31分别表示COM1——COM32是否存在。
//       BIT为0：该串口不存在，BIT为1：该串口存在      
unsigned long comCheck(void)
{
    unsigned long mask;
    int i;
    HANDLE handle;
    char comStr[12];
	
    mask = 0;
	
    for (i = 0; i < 32; i++)
    {
        sprintf(comStr, "\\\\.\\COM%d", i+1);

        handle = CreateFile(comStr, 0, 0,NULL, OPEN_EXISTING, 0, NULL);
				    
        if (handle != INVALID_HANDLE_VALUE)
        {
            mask |= (1ul << i);
            
            CloseHandle(handle);
        }
    }
	
    return mask;
}

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
	InitSerialPort();
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
		m_RfidInfo[i].curtick  = 0;
		m_RfidInfo[i].lasttick = 0;
		m_RfidInfo[i].rfid     = 0;

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

			m_RfidInfo[i].rfid        = (int)atol(buf);
		}
	}
}

void CProcess_demoDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	unsigned long tick,tsp;

	if(nIDEvent == 1001)
	{
		tick = GetTickCount();

		/* 1秒钟刷新一次状态 */
		for(int i=0; i<MAX_CAR_CNT; i++)
		{
			if(!m_gCarInfo[i].carName.IsEmpty() && m_gCarInfo[i].carCurState > CAR_ST_INIT)
			{
				if(m_RfidInfo[i].curtick > 0)
				{
					if(m_gCarInfo[i].carOnline == 0 && m_RfidInfo[i].curtick > m_RfidInfo[i].lasttick)
					{
						m_gCarInfo[i].carOnline = 1;

						m_gCarInfo[i].carCycleCnt ++; /* 计圈加一 */
					}
					else
					{
						m_RfidInfo[i].lasttick  = m_RfidInfo[i].curtick;

						tsp = (int)((tick - m_RfidInfo[i].curtick)/1000);

						if(tsp >= 3)
						{
							/* 连续3秒钟未探测到车辆信号，视为车辆离开出发点 */
							m_gCarInfo[i].carOnline = 0;							
						}
					}
				}

				m_gCarInfo[i].carTimeSec ++;

				if(++m_gCarInfo[i].carTimeSec >= m_gSystemInfo.carMaxTime || m_gCarInfo[i].carCycleCnt >= m_gSystemInfo.carMaxCycle)
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
			cs.Format("%d圈 %d分钟%d秒 ", m_gCarInfo[i].carCycleCnt, (int)(m_gCarInfo[i].carTimeSec)/60, (int)m_gCarInfo[i].carTimeSec%60);
			/*
			if(m_gSystemInfo.carCacType == CAC_BY_CYCLE)
			{
				cs.Format("%d圈", m_gCarInfo[i].carCycleCnt);
			}
			else if(m_gSystemInfo.carCacType == CAC_BY_TIME)
			{
				cs.Format("%d分钟%d秒", (int)(m_gCarInfo[i].carTimeSec)/60, (int)m_gCarInfo[i].carTimeSec%60);
			}
			*/
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
		m_gCarInfo[id].carOnline   = 0;

		m_RfidInfo[id].curtick = m_RfidInfo[id].lasttick = 0;

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

void CProcess_demoDlg::InitSerialPort()
{
	if(FALSE == openCom(1, BAUDRATE_9600))
	{
		AfxMessageBox("通信口打开失败，请检查！");
		return;
	}

	comThreadCreate();
}
