#include "stdafx.h"
#include "ClientDlg.h"

// CClientDlg dialog

CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientDlg::IDD, pParent)
{

}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tabCtrl);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CClientDlg::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// CClientDemoDlg message handlers

BOOL CClientDlg::OnInitDialog()
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
	m_tabCtrl.InsertItem(0, L"视频界面");      //添加参数一选项卡 
	m_tabCtrl.InsertItem(1, L"配置界面");      //添加参数二选项卡

	m_videoDlg.Create(IDD_CLIENTDEMO_DIALOG, GetDlgItem(IDC_TAB1));
	m_configDlg.Create(IDD_DIALOG_CONFIG, GetDlgItem(IDC_TAB1));
	//获得IDC_TABTEST客户区大小
    CRect rs;
    m_tabCtrl.GetClientRect(&rs);
    //调整子对话框在父窗口中的位置
    rs.top += 20;
    rs.left += 1;
    //设置子对话框尺寸并移动到指定位置
    m_videoDlg.MoveWindow(&rs);
	m_configDlg.MoveWindow(&rs);
	
	m_videoDlg.ShowWindow(1);
	m_configDlg.ShowWindow(0);
	m_tabCtrl.SetCurSel(0);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientDlg::OnPaint()
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
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CClientDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int CurSel = m_tabCtrl.GetCurSel();
    switch(CurSel)
    {
     case 0:
		m_videoDlg.ShowWindow(true);
		m_configDlg.ShowWindow(false);
		break;
     case 1:
		m_videoDlg.ShowWindow(false);
		m_configDlg.ShowWindow(true);
		break;
     default:
		break;
    }
	*pResult = 0;
}
