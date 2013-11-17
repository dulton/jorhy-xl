
// ClientDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "VideoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CClientImpl m_impl;

// CClientDemoDlg dialog
CVideoDlg::CVideoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVideoDlg::IDD, pParent)
	, m_sysDate(COleDateTime::GetCurrentTime())
	, m_sysTime(COleDateTime::GetCurrentTime())
	, m_alarmStartDate(COleDateTime::GetCurrentTime())
	, m_alarmStopDate(COleDateTime::GetCurrentTime())
	, m_alarmStopTime(COleDateTime::GetCurrentTime())
{
	int x = sizeof(AlarmInfoResp);
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVideoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_infoList);
	DDX_Control(pDX, IDC_STATIC_1, m_video_1);
	DDX_Control(pDX, IDC_STATIC_2, m_video_2);
	DDX_Control(pDX, IDC_STATIC_3, m_video_3);
	DDX_Control(pDX, IDC_STATIC_4, m_video_4);
	DDX_Control(pDX, IDC_STATIC_5, m_video_5);
	DDX_Control(pDX, IDC_STATIC_6, m_video_6);
	DDX_Control(pDX, IDC_STATIC_7, m_video_7);
	DDX_Control(pDX, IDC_STATIC_8, m_video_8);
	DDX_Control(pDX, IDC_STATIC_Vod1, m_VodWnd1);
	DDX_Control(pDX, IDC_STATIC_Vod2, m_VodWnd2);
	DDX_Control(pDX, IDC_STATIC_Vod3, m_VodWnd3);
	DDX_Control(pDX, IDC_STATIC_Vod4, m_VodWnd4);
	DDX_Control(pDX, IDC_STATIC_Vod5, m_VodWnd5);
	DDX_Control(pDX, IDC_COMBO_Channel, m_VodChannel);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_Date1, m_dateStart);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_Date2, m_dateStop);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_Time1, m_timeStart);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_Time2, m_timeStop);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_SysDate, m_sysDate);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_SysTime, m_sysTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_AlarmStartDate, m_alarmStartDate);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_AlarmStartTime, m_alarmStartTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_AlarmStopDate, m_alarmStopDate);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_AlarmStopTime, m_alarmStopTime);
	DDX_Control(pDX, IDC_STATIC_Vod6, m_VodWnd6);
	DDX_Control(pDX, IDC_STATIC_Vod7, m_VodWnd7);
	DDX_Control(pDX, IDC_STATIC_Vod8, m_VodWnd8);
}

BEGIN_MESSAGE_MAP(CVideoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_1, &CVideoDlg::OnBnClickedBtn1)
	ON_BN_CLICKED(IDC_BTN_2, &CVideoDlg::OnBnClickedBtn2)
	ON_BN_CLICKED(IDC_BTN_3, &CVideoDlg::OnBnClickedBtn3)
	ON_BN_CLICKED(IDC_BTN_4, &CVideoDlg::OnBnClickedBtn4)
	ON_BN_CLICKED(IDC_BTN_5, &CVideoDlg::OnBnClickedBtn5)
	ON_BN_CLICKED(IDC_BTN_6, &CVideoDlg::OnBnClickedBtn6)
	ON_BN_CLICKED(IDC_BTN_7, &CVideoDlg::OnBnClickedBtn7)
	ON_BN_CLICKED(IDC_BTN_8, &CVideoDlg::OnBnClickedBtn8)
	ON_BN_CLICKED(IDC_BTN_VOD_START, &CVideoDlg::OnBnClickedBtnVodStart)
	ON_BN_CLICKED(IDC_BTN_VOD_STOP, &CVideoDlg::OnBnClickedBtnVodStop)
	ON_BN_CLICKED(IDC_BTN_SetTime, &CVideoDlg::OnBnClickedBtnSettime)
	ON_BN_CLICKED(IDC_BTN_GetDevInfo, &CVideoDlg::OnBnClickedBtnGetDevInfo)
	ON_BN_CLICKED(IDC_BTN_GetAlarmInfo, &CVideoDlg::OnBnClickedBtnGetalarminfo)
	ON_BN_CLICKED(IDC_BTN_StopAlarmInfo, &CVideoDlg::OnBnClickedBtnStopalarminfo)
	ON_BN_CLICKED(IDC_BTN_GetSysError, &CVideoDlg::OnBnClickedBtnGetsyserror)
	ON_BN_CLICKED(IDC_BTN_GetLog, &CVideoDlg::OnBnClickedBtnGetLog)
END_MESSAGE_MAP()


// CClientDemoDlg message handlers

BOOL CVideoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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
	m_impl.SetVideoListBox(&m_infoList);
	m_VodChannel.AddString(L"0");
	m_VodChannel.AddString(L"1");
	m_VodChannel.AddString(L"2");
	m_VodChannel.AddString(L"3");
	m_VodChannel.AddString(L"4");
	m_VodChannel.SetCurSel(0);

	time_t tm;
	time(&tm);
	m_dateStart = tm;
	m_dateStop = tm;
	m_timeStart = tm;
	m_timeStop = tm;
	m_sysDate = tm;
	m_sysTime = tm;
	m_alarmStartDate = tm;
	m_alarmStartTime = tm;
	m_alarmStopDate = tm;
	m_alarmStopTime = tm;

	m_bPlay_1 = FALSE;
	m_bPlay_2 = FALSE;
	m_bPlay_3 = FALSE;
	m_bPlay_4 = FALSE;
	m_bPlay_5 = FALSE;
	m_bPlay_6 = FALSE;
	m_bPlay_7 = FALSE;
	m_bPlay_8 = FALSE;

	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVideoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVideoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVideoDlg::OnBnClickedBtn1()
{
	// TODO: Add your control notification handler code here
	if (!m_bPlay_1)
	{
		m_impl.RealPlay("XLL325487EDC", 0, m_video_1.m_hWnd);
		m_bPlay_1 = TRUE;
	}
	else
	{
		m_impl.RealStop("XLL325487EDC", 0);
		m_bPlay_1 = FALSE;
	}
}


void CVideoDlg::OnBnClickedBtn2()
{
	// TODO: Add your control notification handler code here
	if (!m_bPlay_2)
	{
		m_impl.RealPlay("XLL325487EDC", 1, m_video_2.m_hWnd);
		m_bPlay_2 = TRUE;
	}
	else
	{
		m_impl.RealStop("XLL325487EDC", 1);
		m_bPlay_2 = FALSE;
	}
}


void CVideoDlg::OnBnClickedBtn3()
{
	// TODO: Add your control notification handler code here
	if (!m_bPlay_3)
	{
		m_impl.RealPlay("XLL325487EDC", 2, m_video_3.m_hWnd);
		m_bPlay_3 = TRUE;
	}
	else
	{
		m_impl.RealStop("XLL325487EDC", 2);
		m_bPlay_3 = FALSE;
	}
}


void CVideoDlg::OnBnClickedBtn4()
{
	// TODO: Add your control notification handler code here
	if (!m_bPlay_4)
	{
		m_impl.RealPlay("XLL325487EDC", 3, m_video_8.m_hWnd);
		m_bPlay_4 = TRUE;
	}
	else
	{
		m_impl.RealStop("XLL325487EDC", 3);
		m_bPlay_4 = FALSE;
	}
}


void CVideoDlg::OnBnClickedBtn5()
{
	// TODO: Add your control notification handler code here
	if (!m_bPlay_5)
	{
		m_impl.RealPlay("CD508893472E", 4, m_video_4.m_hWnd);
		m_bPlay_5 = TRUE;
	}
	else
	{
		m_impl.RealStop("CD508893472E", 4);
		m_bPlay_5 = FALSE;
	}
}


void CVideoDlg::OnBnClickedBtn6()
{
	// TODO: Add your control notification handler code here
	if (!m_bPlay_6)
	{
		m_impl.RealPlay("CD508893472E", 5, m_video_5.m_hWnd);
		m_bPlay_6 = TRUE;
	}
	else
	{
		m_impl.RealStop("CD508893472E", 5);
		m_bPlay_6 = FALSE;
	}
}


void CVideoDlg::OnBnClickedBtn7()
{
	// TODO: Add your control notification handler code here
	if (!m_bPlay_7)
	{
		m_impl.RealPlay("CD508893472E", 6, m_video_6.m_hWnd);
		m_bPlay_7 = TRUE;
	}
	else
	{
		m_impl.RealStop("CD508893472E", 6);
		m_bPlay_7 = FALSE;
	}
}


void CVideoDlg::OnBnClickedBtn8()
{
	// TODO: Add your control notification handler code here
	if (!m_bPlay_8)
	{
		m_impl.RealPlay("CD508893472Es", 7, m_video_7.m_hWnd);
		m_bPlay_8 = TRUE;
	}
	else
	{
		m_impl.RealStop("CD508893472E", 7);
		m_bPlay_8 = FALSE;
	}
}


void CVideoDlg::OnBnClickedBtnVodStart()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CTime tempStartTime(m_dateStart.GetYear()
		,m_dateStart.GetMonth()
		,m_dateStart.GetDay()
		,m_timeStart.GetHour()
		,m_timeStart.GetMinute()
		,m_timeStart.GetSecond());

	CTime tempEndTime(m_dateStop.GetYear()
		,m_dateStop.GetMonth()
		,m_dateStop.GetDay()
		,m_timeStop.GetHour()
		,m_timeStop.GetMinute()
		,m_timeStop.GetSecond());

	int nChannel = m_VodChannel.GetCurSel();
	HWND hwnd = NULL;
	switch (nChannel)
	{
	case 0:
		hwnd = m_VodWnd1.m_hWnd;
		break;
	case 1:
		hwnd = m_VodWnd2.m_hWnd;
		break;
	case 2:
		hwnd = m_VodWnd3.m_hWnd;
		break;
	case 3:
		hwnd = m_VodWnd4.m_hWnd;
		break;
	case 4:
		hwnd = m_VodWnd5.m_hWnd;
		break;
	default:
		break;
	}
	GUID sessionId;
	CoCreateGuid(&sessionId);
	m_vodMap[nChannel] = sessionId;
	m_impl.VodPlay("XLL325487EDC", nChannel, sessionId, hwnd, tempStartTime.GetTime(), tempEndTime.GetTime());
}


void CVideoDlg::OnBnClickedBtnVodStop()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int nChannel = m_VodChannel.GetCurSel();
	std::map<int, GUID>::iterator it = m_vodMap.find(nChannel);
	if (it != m_vodMap.end())
		m_impl.VodStop("XLL325487EDC", nChannel, it->second);
}

void CVideoDlg::OnBnClickedBtnSettime()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CTime tempSysTime(m_sysDate.GetYear()
		,m_sysDate.GetMonth()
		,m_sysDate.GetDay()
		,m_sysTime.GetHour()
		,m_sysTime.GetMinute()
		,m_sysTime.GetSecond());

	m_impl.SetTime(tempSysTime.GetTime());
}


void CVideoDlg::OnBnClickedBtnGetDevInfo()
{
	// TODO: Add your control notification handler code here
	m_impl.GetDevInfo("XLL325487EDC");
}


void CVideoDlg::OnBnClickedBtnGetalarminfo()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CTime tempStartTime(m_alarmStartDate.GetYear()
		,m_alarmStartDate.GetMonth()
		,m_alarmStartDate.GetDay()
		,m_alarmStartTime.GetHour()
		,m_alarmStartTime.GetMinute()
		,m_alarmStartTime.GetSecond());

	CTime tempEndTime(m_alarmStopDate.GetYear()
		,m_alarmStopDate.GetMonth()
		,m_alarmStopDate.GetDay()
		,m_alarmStopTime.GetHour()
		,m_alarmStopTime.GetMinute()
		,m_alarmStopTime.GetSecond());
	m_impl.GetAlarmInfo("XLL325487EDC", tempStartTime.GetTime(), tempEndTime.GetTime());
}

void CVideoDlg::OnBnClickedBtnStopalarminfo()
{
	// TODO: Add your control notification handler code here
	m_impl.StopAlarmInfo("XLL325487EDC");
}


void CVideoDlg::OnBnClickedBtnGetsyserror()
{
	// TODO: Add your control notification handler code here
}


void CVideoDlg::OnBnClickedBtnGetLog()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CTime tempStartTime(m_alarmStartDate.GetYear()
		,m_alarmStartDate.GetMonth()
		,m_alarmStartDate.GetDay()
		,m_alarmStartTime.GetHour()
		,m_alarmStartTime.GetMinute()
		,m_alarmStartTime.GetSecond());

	CTime tempEndTime(m_alarmStopDate.GetYear()
		,m_alarmStopDate.GetMonth()
		,m_alarmStopDate.GetDay()
		,m_alarmStopTime.GetHour()
		,m_alarmStopTime.GetMinute()
		,m_alarmStopTime.GetSecond());
	m_impl.GetLogInfo("XLL325487EDC", tempStartTime.GetTime(), tempEndTime.GetTime());
}
