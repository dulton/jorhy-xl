#pragma once
#include "afxwin.h"


// CLogin dialog

class CLoginDlg : public CDialogEx
{
	//DECLARE_DYNAMIC(CLogin)

public:
	CLoginDlg(CWnd* pParent = NULL);   // standard constructor
	//virtual ~CLogin();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;
	BOOL m_bLogin;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CIPAddressCtrl m_ipAddr;
	CEdit m_strPort;
	CString m_strUser;
	CString m_strPasswd;
	afx_msg void OnBnClickedBtnLogin();
	afx_msg void OnBnClickedBtnLogout();
};