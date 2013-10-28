#pragma once
#include "afxwin.h"
#include "afxdtctl.h"
#include "atlcomtime.h"
#include "afxcmn.h"
#include "resource.h"
// CConfigDlg dialog

class CConfigDlg : public CDialogEx
{
	//DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_DIALOG_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;
	CListBox m_infoList;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnDvrtotleinfo();
	afx_msg void OnBnClickedBtnUsertotleinfo();
	afx_msg void OnBnClickedBtngetdvrlist();
	afx_msg void OnBnClickedBtnGetuserlist();
	afx_msg void OnBnClickedBtnGetdepartmentinfo();
};
