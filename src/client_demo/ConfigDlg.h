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
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
