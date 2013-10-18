
// ClientDemoDlg.h : header file
//

#pragma once
#include "ClientDemoImpl.h"
#include "afxwin.h"

// CClientDemoDlg dialog
class CClientDemoDlg : public CDialogEx
{
// Construction
public:
	CClientDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CLIENTDEMO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CClientDemoImpl m_impl;
	CListBox m_infoList;
	BOOL m_bLogin;
	BOOL m_bPlay_1;
	BOOL m_bPlay_2;
	BOOL m_bPlay_3;
	BOOL m_bPlay_4;
	BOOL m_bPlay_5;
	BOOL m_bPlay_6;
	BOOL m_bPlay_7;
	BOOL m_bPlay_8;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnLogin();
	afx_msg void OnBnClickedBtnLogout();
	afx_msg void OnBnClickedBtn1();
	afx_msg void OnBnClickedBtn2();
	afx_msg void OnBnClickedBtn3();
	afx_msg void OnBnClickedBtn4();
	afx_msg void OnBnClickedBtn5();
	afx_msg void OnBnClickedBtn6();
	afx_msg void OnBnClickedBtn7();
	afx_msg void OnBnClickedBtn8();
	CStatic m_video_1;
	CStatic m_video_2;
	CStatic m_video_3;
	CStatic m_video_4;
	CStatic m_video_5;
	CStatic m_video_6;
	CStatic m_video_7;
	CStatic m_video_8;
};
