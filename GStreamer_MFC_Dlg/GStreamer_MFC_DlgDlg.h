
// GStreamer_MFC_DlgDlg.h : header file
//

#pragma once
//#include "tcpSocket.h"
#include "../Common/GStreamer_MFC/_TCP_/tcpSocket.h"

#include "tcp/Client_TCP.h"

#include "../Common/GStreamer_MFC/ENCRYPTO/SHA256/sha256.h"

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
	
	// Generated message map functions
HICON m_hIcon;
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
	CCLIENT_CONTROL* clnt_con;
	CTCP_SOCKET socket;
};
