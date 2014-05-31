// Login.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "ClientImpl.h"

CClientImpl m_impl;

// CLogin dialog

//IMPLEMENT_DYNAMIC(CLogin, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoginDlg::IDD, pParent)
	, m_strUser(_T(""))
	, m_strPasswd(_T(""))
{
	m_bLogin = FALSE;
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_Ip, m_ipAddr);
	DDX_Control(pDX, IDC_EDIT_Port, m_strPort);
	DDX_Text(pDX, IDC_EDIT_USER, m_strUser);
	DDX_Text(pDX, IDC_EDIT_PASSWD, m_strPasswd);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_Login, &CLoginDlg::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_BTN_Logout, &CLoginDlg::OnBnClickedBtnLogout)
END_MESSAGE_MAP()


// CLogin message handlers
BOOL CLoginDlg::OnInitDialog()
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
	m_ipAddr.SetAddress(222, 214, 218, 237);
	m_strPort.SetCueBanner(L"8502");
	m_strUser.SetString(L"test1");
	m_strPasswd.SetString(L"test1");
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLoginDlg::OnPaint()
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
HCURSOR CLoginDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLoginDlg::OnBnClickedBtnLogin()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	DWORD dwAddr = 0; 
	m_ipAddr.GetAddress(dwAddr);
	char userName[32] = {0};
	sprintf_s(userName, "%S", (LPSTR)(LPCTSTR)m_strUser);
	char passWord[32] = {0};
	sprintf_s(passWord, "%S", (LPSTR)(LPCTSTR)m_strPasswd);
	m_impl.Login(dwAddr, _wtoi(m_strPort.GetCueBanner().GetString()), userName, passWord, 1);
	m_bLogin = TRUE;
	CDialog::OnOK();
}


void CLoginDlg::OnBnClickedBtnLogout()
{
	// TODO: Add your control notification handler code here
	if (m_bLogin)
	{
		UpdateData(TRUE);
		m_impl.Logout((LPSTR)(LPCTSTR)m_strUser, (LPSTR)(LPCTSTR)m_strPasswd);
	}
	CDialog::OnCancel();
}
