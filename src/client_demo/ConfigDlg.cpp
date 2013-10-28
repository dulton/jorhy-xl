#include "stdafx.h"
#include "ConfigDlg.h"
#include "ClientImpl.h"

extern CClientImpl m_impl;

// CClientDlg dialog
CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CConfigDlg::IDD, pParent)
{

}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_Config, m_infoList);
}

BEGIN_MESSAGE_MAP(CConfigDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_DvrTotleInfo, &CConfigDlg::OnBnClickedBtnDvrtotleinfo)
	ON_BN_CLICKED(IDC_BTN_UserTotleInfo, &CConfigDlg::OnBnClickedBtnUsertotleinfo)
	ON_BN_CLICKED(IDC_BTNGetDvrList, &CConfigDlg::OnBnClickedBtngetdvrlist)
	ON_BN_CLICKED(IDC_BTN_GetUserList, &CConfigDlg::OnBnClickedBtnGetuserlist)
END_MESSAGE_MAP()


// CClientDemoDlg message handlers

BOOL CConfigDlg::OnInitDialog()
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
	m_impl.SetConfigListBox(&m_infoList);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CConfigDlg::OnPaint()
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
HCURSOR CConfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CConfigDlg::OnBnClickedBtnDvrtotleinfo()
{
	// TODO: Add your control notification handler code here
	m_impl.GetDvrTotleInfo();
}


void CConfigDlg::OnBnClickedBtnUsertotleinfo()
{
	// TODO: Add your control notification handler code here
	m_impl.GetUserTotleInfo();
}


void CConfigDlg::OnBnClickedBtngetdvrlist()
{
	// TODO: Add your control notification handler code here
	m_impl.GetDvrList(1, 1);
}


void CConfigDlg::OnBnClickedBtnGetuserlist()
{
	// TODO: Add your control notification handler code here
	m_impl.GetUserList(0);
}
