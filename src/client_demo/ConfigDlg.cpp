#include "stdafx.h"
#include "ConfigDlg.h"
#include "ClientImpl.h"

extern CClientImpl m_impl;

// CClientDlg dialog
CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CConfigDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDD_CONFIG);
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_Config, m_infoList);
	DDX_Text(pDX, IDC_EDITDeviceID, m_strDevicId);
	DDX_Text(pDX, IDC_EDITDeviceNum, m_strDevicNum);
	DDX_Text(pDX, IDC_EDITPhoneNum, m_strPhoneNum);
	DDX_Text(pDX, IDC_EDITChannelNum, m_strChannelNum);
	DDX_Text(pDX, IDC_EDITDepartmentID, m_strDvrDepartmentId);
	DDX_Control(pDX, IDC_RADIO_OnLine, m_btnDvrOnlien);
	DDX_Control(pDX, IDC_RADIO_State, m_btnDvrState);
	DDX_Text(pDX, IDC_UserId, m_strUserId);
	DDX_Text(pDX, IDC_Name, m_strName);
	DDX_Text(pDX, IDC_EDITUserName, m_strUserName);
	DDX_Text(pDX, IDC_EDITPassWord, m_strPassWord);
	DDX_Text(pDX, IDCUserDepartmentID, m_strUserDepartmentId);
	DDX_Control(pDX, IDC_RADIO_UserOnLine, m_btnUserOnlien);
	DDX_Text(pDX, IDC_DepartmentId, m_strDepartmentId);
	DDX_Text(pDX, IDC_ParentId, m_strParentId);
	DDX_Text(pDX, IDC_Address, m_strAddress);
	DDX_Text(pDX, IDC_Order, m_strOrder);
	DDX_Control(pDX, IDC_RADIO_DepartmentOnLine, m_btnDepartmentState);
}

BEGIN_MESSAGE_MAP(CConfigDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_USER_UPDATEDATA, &CConfigDlg::OnUpdateDataFalse)
	ON_BN_CLICKED(IDC_BTN_DvrTotleInfo, &CConfigDlg::OnBnClickedBtnDvrtotleinfo)
	ON_BN_CLICKED(IDC_BTN_UserTotleInfo, &CConfigDlg::OnBnClickedBtnUsertotleinfo)
	ON_BN_CLICKED(IDC_BTNGetDvrList, &CConfigDlg::OnBnClickedBtngetdvrlist)
	ON_BN_CLICKED(IDC_BTN_GetUserList, &CConfigDlg::OnBnClickedBtnGetuserlist)
	ON_BN_CLICKED(IDC_BTN_GetDepartmentInfo, &CConfigDlg::OnBnClickedBtnGetdepartmentinfo)
	ON_BN_CLICKED(IDC_BTN_AddDvrInfo, &CConfigDlg::OnBnClickedBtnAdddvrinfo)
	ON_BN_CLICKED(IDC_BTN_GetDvrInfo, &CConfigDlg::OnBnClickedBtnGetdvrinfo)
	ON_BN_CLICKED(IDC_BTN_GetUserInfo, &CConfigDlg::OnBnClickedBtnGetuserinfo)
	ON_BN_CLICKED(IDC_BTN_DpInfo, &CConfigDlg::OnBnClickedBtnGetDepartmentinfo)
	ON_BN_CLICKED(IDC_BTN_MFDvrInfo, &CConfigDlg::OnBnClickedBtnMfdvrinfo)
	ON_BN_CLICKED(IDC_BTN_DelDVR, &CConfigDlg::OnBnClickedBtnDeldvr)
	ON_BN_CLICKED(IDC_BTN_AddUserInfo, &CConfigDlg::OnBnClickedBtnAdduserinfo)
	ON_BN_CLICKED(IDC_BTN_MFUserInfo, &CConfigDlg::OnBnClickedBtnMfuserinfo)
	ON_BN_CLICKED(IDC_BTN_DelUser, &CConfigDlg::OnBnClickedBtnDeluser)
	ON_BN_CLICKED(IDC_BTN_AddDpInfo, &CConfigDlg::OnBnClickedBtnAddDepartmentInfo)
	ON_BN_CLICKED(IDC_BTN_MFDpInfo, &CConfigDlg::OnBnClickedBtnMfDepartmentInfo)
	ON_BN_CLICKED(IDC_BTN_DelDepartment, &CConfigDlg::OnBnClickedBtnDeldepartment)
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
	m_impl.SetConfigDlg(this);
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

LRESULT CConfigDlg::OnUpdateDataFalse(WPARAM wParam, LPARAM lParam)
{
	UpdateData(FALSE);
	return 0;
}

void CConfigDlg::GetDvrInfo(DVRInfo &dvrInfo)
{
	UpdateData(TRUE);
	memset (&dvrInfo, 0, sizeof(DVRInfo));
	sprintf_s(dvrInfo.szEquipmentID, "%S", (LPSTR)(LPCTSTR)m_strDevicId);
	sprintf_s(dvrInfo.szVehicleNO, "%S", (LPSTR)(LPCTSTR)m_strDevicNum);
	sprintf_s(dvrInfo.szPhone, "%S", (LPSTR)(LPCTSTR)m_strPhoneNum);
	dvrInfo.nTotalChannels = _wtoi((LPCTSTR)m_strChannelNum);
	dvrInfo.lDepartmentID = _wtoi((LPCTSTR)m_strDvrDepartmentId);
	dvrInfo.nOnline = m_btnDvrOnlien.GetCheck();
	dvrInfo.nState = m_btnDvrState.GetCheck();
}

void CConfigDlg::SetDvrInfo(const DVRInfo &dvrInfo)
{
	try
	{
		m_strDevicId.Format(L"%S", dvrInfo.szEquipmentID);
		m_strDevicNum.Format(L"%S", dvrInfo.szVehicleNO);
		m_strPhoneNum.Format(L"%S", dvrInfo.szPhone);
		m_strChannelNum.Format(L"%d", dvrInfo.nTotalChannels);
		m_strDvrDepartmentId.Format(L"%d", dvrInfo.lDepartmentID);
		m_btnDvrOnlien.SetCheck(dvrInfo.nOnline);
		m_btnDvrState.SetCheck(dvrInfo.nState);
		PostMessage(WM_USER_UPDATEDATA);
	}
	catch (...){}
}

void CConfigDlg::GetUserInfo(UserInfo &userInfo)
{
	UpdateData(TRUE);
	memset (&userInfo, 0, sizeof(UserInfo));
	userInfo.lUserID = _wtoi((LPCTSTR)m_strUserId);
	sprintf_s(userInfo.szName, "%S", (LPSTR)(LPCTSTR)m_strName);
	sprintf_s(userInfo.szAccountName, "%S", (LPSTR)(LPCTSTR)m_strUserName);
	sprintf_s(userInfo.szPassword, "%S", (LPSTR)(LPCTSTR)m_strPassWord);
	userInfo.lDepartmentID = _wtoi((LPCTSTR)m_strUserDepartmentId);
	userInfo.nState = m_btnUserOnlien.GetCheck();
}

void CConfigDlg::SetUserInfo(const UserInfo &userInfo)
{
	try 
	{
		m_strUserId.Format(L"%d", userInfo.lUserID);
		WCHAR wszVehicleNum[128] = {0};
		MultiByteToWideChar(CP_ACP, 0, userInfo.szName, strlen(userInfo.szName), wszVehicleNum, 128);
		m_strName.Format(L"%s", wszVehicleNum);
		m_strUserName.Format(L"%S", userInfo.szAccountName);
		m_strPassWord.Format(L"%S", userInfo.szPassword);
		m_strUserDepartmentId.Format(L"%d", userInfo.lDepartmentID);
		m_btnDvrOnlien.SetCheck(userInfo.nState);
		PostMessage(WM_USER_UPDATEDATA);
	}
	catch(...){}
}

void CConfigDlg::GetDepartmentInfo(DepartmentInfo &departmentInfo)
{
	UpdateData(TRUE);
	memset (&departmentInfo, 0, sizeof(DepartmentInfo));
	departmentInfo.lDepartmentID = _wtoi((LPCTSTR)m_strDepartmentId);
	sprintf_s(departmentInfo.szName, "%S", (LPSTR)(LPCTSTR)m_strAddress);
	departmentInfo.ParentID = _wtoi((LPCTSTR)m_strParentId);
	departmentInfo.OrderIndex = _wtoi((LPCTSTR)m_strOrder);
	departmentInfo.nState = m_btnDepartmentState.GetCheck();
}

void CConfigDlg::SetDepartmentInfo(const DepartmentInfo &departmentInfo)
{
	try 
	{
		WCHAR wszVehicleNum[128] = {0};
		MultiByteToWideChar(CP_ACP, 0, departmentInfo.szName, strlen(departmentInfo.szName), wszVehicleNum, 128);
		m_strDepartmentId.Format(L"%d", departmentInfo.lDepartmentID);
		m_strAddress.Format(_T("%s"), wszVehicleNum);
		m_strParentId.Format(L"%d", departmentInfo.ParentID);
		m_strOrder.Format(L"%d", departmentInfo.OrderIndex);
		m_btnDepartmentState.SetCheck(departmentInfo.nState);
		PostMessage(WM_USER_UPDATEDATA);
	}
	catch(...) {}
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
	m_impl.GetUserList(0, 1);
}


void CConfigDlg::OnBnClickedBtnGetdepartmentinfo()
{
	// TODO: Add your control notification handler code here
	m_impl.GetDepartmentList();
}


void CConfigDlg::OnBnClickedBtnAdddvrinfo()
{
	// TODO: Add your control notification handler code here
	DVRInfo info = {0};
	GetDvrInfo(info);
	m_impl.AddDvrInfo(info);
}


void CConfigDlg::OnBnClickedBtnGetdvrinfo()
{
	// TODO: Add your control notification handler code here
	DVRInfo info = {0};
	GetDvrInfo(info);
	m_impl.GetDvrInfo(info);
}

void CConfigDlg::OnBnClickedBtnMfdvrinfo()
{
	// TODO: Add your control notification handler code here
	DVRInfo info = {0};
	GetDvrInfo(info);
	m_impl.ModifyDvrInfo(info);
}

void CConfigDlg::OnBnClickedBtnDeldvr()
{
	// TODO: Add your control notification handler code here
	DVRInfo info = {0};
	GetDvrInfo(info);
	m_impl.DeleteDvrInfo(info);
}

void CConfigDlg::OnBnClickedBtnAdduserinfo()
{
	// TODO: Add your control notification handler code here
	UserInfo info = {0};
	GetUserInfo(info);
	m_impl.AddUserInfo(info);
}

void CConfigDlg::OnBnClickedBtnGetuserinfo()
{
	// TODO: Add your control notification handler code here
	UserInfo info = {0};
	GetUserInfo(info);
	m_impl.GetUserInfo(info);
}

void CConfigDlg::OnBnClickedBtnMfuserinfo()
{
	// TODO: Add your control notification handler code here
	UserInfo info = {0};
	GetUserInfo(info);
	m_impl.ModifyUserInfo(info);
}

void CConfigDlg::OnBnClickedBtnDeluser()
{
	// TODO: Add your control notification handler code here
	UserInfo info = {0};
	GetUserInfo(info);
	m_impl.DeleteUserInfo(info);
}

void CConfigDlg::OnBnClickedBtnAddDepartmentInfo()
{
	// TODO: Add your control notification handler code here
	DepartmentInfo info = {0};
	GetDepartmentInfo(info);
	m_impl.AddDepartmentInfo(info);
}

void CConfigDlg::OnBnClickedBtnGetDepartmentinfo()
{
	// TODO: Add your control notification handler code here
	DepartmentInfo info = {0};
	GetDepartmentInfo(info);
	m_impl.GetDepartmentInfo(info);
}

void CConfigDlg::OnBnClickedBtnMfDepartmentInfo()
{
	// TODO: Add your control notification handler code here
	DepartmentInfo info = {0};
	GetDepartmentInfo(info);
	m_impl.ModifyDepartmentInfo(info);
}

void CConfigDlg::OnBnClickedBtnDeldepartment()
{
	// TODO: Add your control notification handler code here
	DepartmentInfo info = {0};
	GetDepartmentInfo(info);
	m_impl.DeleteDepartmentInfo(info);
}
