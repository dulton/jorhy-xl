#pragma once
#include "afxwin.h"
#include "afxdtctl.h"
#include "atlcomtime.h"
#include "afxcmn.h"
#include "resource.h"
#include "XlClientType.h"
// CConfigDlg dialog
#define WM_USER_UPDATEDATA WM_USER+100
class CConfigDlg : public CDialogEx
{
	//DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;
	CListBox m_infoList;
	DVRInfo m_dvrInfo;
	UserInfo m_userInfo;
	DepartmentInfo m_departmentInfo;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnUpdateDataFalse(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	void GetDvrInfo(DVRInfo &dvrInfo);
	void SetDvrInfo(const DVRInfo &dvrInfo);
	void GetUserInfo(UserInfo &userInfo);
	void SetUserInfo(const UserInfo &userInfo);
	void GetDepartmentInfo(DepartmentInfo &departmentInfo);
	void SetDepartmentInfo(const DepartmentInfo &departmentInfo);

public:
	CString m_strDevicId;
	CString m_strDevicNum;
	CString m_strPhoneNum;
	CString m_strChannelNum;
	CString m_strDvrDepartmentId;
	CButton m_btnDvrOnlien;
	CButton  m_btnDvrState;
	CString m_strUserId;
	CString m_strName;
	CString m_strUserName;
	CString m_strPassWord;
	CString m_strUserDepartmentId;
	CButton m_btnUserOnlien;
	CString m_strDepartmentId;
	CString m_strParentId;
	CString m_strAddress;
	CString m_strOrder;
	CButton m_btnDepartmentState;
	afx_msg void OnBnClickedBtnDvrtotleinfo();
	afx_msg void OnBnClickedBtnUsertotleinfo();
	afx_msg void OnBnClickedBtngetdvrlist();
	afx_msg void OnBnClickedBtnGetuserlist();
	afx_msg void OnBnClickedBtnGetdepartmentinfo();
	afx_msg void OnBnClickedBtnAdddvrinfo();
	afx_msg void OnBnClickedBtnGetdvrinfo();
	afx_msg void OnBnClickedBtnGetuserinfo();
	afx_msg void OnBnClickedBtnGetDepartmentinfo();
	afx_msg void OnBnClickedBtnMfdvrinfo();
	afx_msg void OnBnClickedBtnDeldvr();
	afx_msg void OnBnClickedBtnAdduserinfo();
	afx_msg void OnBnClickedBtnMfuserinfo();
	afx_msg void OnBnClickedBtnDeluser();
	afx_msg void OnBnClickedBtnAddDepartmentInfo();
	afx_msg void OnBnClickedBtnMfDepartmentInfo();
	afx_msg void OnBnClickedBtnDeldepartment();
};
