
// ClientDemoDlg.h : header file
//

#pragma once
#include "ClientImpl.h"
#include "afxwin.h"
#include "afxdtctl.h"
#include "atlcomtime.h"
#include "afxcmn.h"

// CClientDemoDlg dialog
class CVideoDlg : public CDialogEx
{
// Construction
public:
	CVideoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CLIENTDEMO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CListBox m_infoList;
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
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtn1();
	afx_msg void OnBnClickedBtn2();
	afx_msg void OnBnClickedBtn3();
	afx_msg void OnBnClickedBtn4();
	afx_msg void OnBnClickedBtn5();
	afx_msg void OnBnClickedBtn6();
	afx_msg void OnBnClickedBtn7();
	afx_msg void OnBnClickedBtn8();
	afx_msg void OnBnClickedBtnVodStart();
	afx_msg void OnBnClickedBtnVodStop();
	afx_msg void OnBnClickedBtnSettime();
	afx_msg void OnBnClickedBtnGetDevInfo();
	afx_msg void OnBnClickedBtnGetalarminfo();
	afx_msg void OnBnClickedBtnStopalarminfo();
	afx_msg void OnBnClickedBtnGetsyserror();
	afx_msg void OnBnClickedBtnGetLog();
	CStatic m_video_1;
	CStatic m_video_2;
	CStatic m_video_3;
	CStatic m_video_4;
	CStatic m_video_5;
	CStatic m_video_6;
	CStatic m_video_7;
	CStatic m_video_8;
	CStatic m_VodWnd1;
	CStatic m_VodWnd2;
	CStatic m_VodWnd3;
	CStatic m_VodWnd4;
	CStatic m_VodWnd5;
	CComboBox m_VodChannel;
	CTime m_dateStart;
	CTime m_dateStop;
	CTime m_timeStart;
	CTime m_timeStop;
	CTime m_sysDate;
	CTime m_sysTime;
	CTime m_alarmStartDate;
	CTime m_alarmStartTime;
	CTime m_alarmStopDate;
	CTime m_alarmStopTime;
	CStatic m_VodWnd6;
	CStatic m_VodWnd7;
	CStatic m_VodWnd8;
};
