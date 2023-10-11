
// GStreamer_MFC_DlgDlg.h : header file
//

#pragma once
//#include "tcpSocket.h"
#include "../GStreamer_MFC_Server/tcp/tcpSocket.h"

// CGStreamerMFCDlgDlg dialog
class CGStreamerMFCDlgDlg : public CDialog
{
// Construction
public:
	CGStreamerMFCDlgDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LOGIN };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CString m_loginID;
	CString m_loginPassword;
	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnBnClickedButtonJoinMembership();
};
