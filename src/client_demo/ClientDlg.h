#pragma once
#include "afxwin.h"
#include "afxdtctl.h"
#include "atlcomtime.h"
#include "afxcmn.h"
#include "resource.h"
#include "VideoDlg.h"
#include "ConfigDlg.h"

// CClientDlg dialog

class CClientDlg : public CDialogEx
{
	//DECLARE_DYNAMIC(CClientDlg)

public:
	CClientDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_CLIENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	//afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_tabCtrl;
	CVideoDlg m_videoDlg;
	CConfigDlg m_configDlg;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
};
