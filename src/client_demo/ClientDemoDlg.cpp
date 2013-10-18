
// ClientDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClientDemo.h"
#include "ClientDemoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CClientDemoDlg dialog



CClientDemoDlg::CClientDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientDemoDlg::IDD, pParent)
	, m_impl(&m_infoList)
{
	m_bLogin = FALSE;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientDemoDlg::DoDataExchange(CDataExchange* pDX)
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
}

BEGIN_MESSAGE_MAP(CClientDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_Login, &CClientDemoDlg::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_BTN_Logout, &CClientDemoDlg::OnBnClickedBtnLogout)
	ON_BN_CLICKED(IDC_BTN_1, &CClientDemoDlg::OnBnClickedBtn1)
	ON_BN_CLICKED(IDC_BTN_2, &CClientDemoDlg::OnBnClickedBtn2)
	ON_BN_CLICKED(IDC_BTN_3, &CClientDemoDlg::OnBnClickedBtn3)
	ON_BN_CLICKED(IDC_BTN_4, &CClientDemoDlg::OnBnClickedBtn4)
	ON_BN_CLICKED(IDC_BTN_5, &CClientDemoDlg::OnBnClickedBtn5)
	ON_BN_CLICKED(IDC_BTN_6, &CClientDemoDlg::OnBnClickedBtn6)
	ON_BN_CLICKED(IDC_BTN_7, &CClientDemoDlg::OnBnClickedBtn7)
	ON_BN_CLICKED(IDC_BTN_8, &CClientDemoDlg::OnBnClickedBtn8)
END_MESSAGE_MAP()


// CClientDemoDlg message handlers

BOOL CClientDemoDlg::OnInitDialog()
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

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientDemoDlg::OnPaint()
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
HCURSOR CClientDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CClientDemoDlg::OnBnClickedBtnLogin()
{
	// TODO: Add your control notification handler code here
	if (!m_bLogin)
	{
		m_impl.Login("admin", "admin");
		m_bPlay_1 = FALSE;
		m_bPlay_2 = FALSE;
		m_bPlay_3 = FALSE;
		m_bPlay_4 = FALSE;
		m_bPlay_5 = FALSE;
		m_bPlay_6 = FALSE;
		m_bPlay_7 = FALSE;
		m_bPlay_8 = FALSE;
		m_bLogin = TRUE;
	}
}

void CClientDemoDlg::OnBnClickedBtnLogout()
{
	// TODO: Add your control notification handler code here
	if (m_bLogin)
	{
		m_impl.Logout("admin", "admin");
		m_bPlay_1 = FALSE;
		m_bPlay_2 = FALSE;
		m_bPlay_3 = FALSE;
		m_bPlay_4 = FALSE;
		m_bPlay_5 = FALSE;
		m_bPlay_6 = FALSE;
		m_bPlay_7 = FALSE;
		m_bPlay_8 = FALSE;
		m_bLogin = FALSE;
	}
}


void CClientDemoDlg::OnBnClickedBtn1()
{
	// TODO: Add your control notification handler code here
	if (m_bLogin)
	{
		if (!m_bPlay_1)
		{
			m_impl.RealPlay("CD508893472E", 0, m_video_1.m_hWnd);
			m_bPlay_1 = TRUE;
		}
		else
		{
			m_impl.RealStop("CD508893472E", 0);
			m_bPlay_1 = FALSE;
		}
	}
}


void CClientDemoDlg::OnBnClickedBtn2()
{
	// TODO: Add your control notification handler code here
	if (m_bLogin)
	{
		if (!m_bPlay_2)
		{
			m_impl.RealPlay("CD508893472E", 1, m_video_2.m_hWnd);
			m_bPlay_2 = TRUE;
		}
		else
		{
			m_impl.RealStop("CD508893472E", 1);
			m_bPlay_2 = FALSE;
		}
	}
}


void CClientDemoDlg::OnBnClickedBtn3()
{
	// TODO: Add your control notification handler code here
	if (m_bLogin)
	{
		if (!m_bPlay_3)
		{
			m_impl.RealPlay("CD508893472E", 2, m_video_3.m_hWnd);
			m_bPlay_3 = TRUE;
		}
		else
		{
			m_impl.RealStop("CD508893472E", 2);
			m_bPlay_3 = FALSE;
		}
	}

}


void CClientDemoDlg::OnBnClickedBtn4()
{
	// TODO: Add your control notification handler code here
	if (m_bLogin)
	{
		if (!m_bPlay_4)
		{
			m_impl.RealPlay("CD508893472E", 3, m_video_8.m_hWnd);
			m_bPlay_4 = TRUE;
		}
		else
		{
			m_impl.RealStop("CD508893472E", 3);
			m_bPlay_4 = FALSE;
		}
	}

}


void CClientDemoDlg::OnBnClickedBtn5()
{
	// TODO: Add your control notification handler code here
	if (m_bLogin)
	{
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
}


void CClientDemoDlg::OnBnClickedBtn6()
{
	// TODO: Add your control notification handler code here
	if (m_bLogin)
	{
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
}


void CClientDemoDlg::OnBnClickedBtn7()
{
	// TODO: Add your control notification handler code here
	if (m_bLogin)
	{
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
}


void CClientDemoDlg::OnBnClickedBtn8()
{
	// TODO: Add your control notification handler code here
	if (m_bLogin)
	{
		if (!m_bPlay_8)
		{
			m_impl.RealPlay("CD508893472E", 7, m_video_7.m_hWnd);
			m_bPlay_8 = TRUE;
		}
		else
		{
			m_impl.RealStop("CD508893472E", 7);
			m_bPlay_8 = FALSE;
		}
	}
}
