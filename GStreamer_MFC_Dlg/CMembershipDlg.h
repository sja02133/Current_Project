#pragma once

#include "tcp/Client_TCP.h"

// CMembershipDlg dialog

class CMembershipDlg : public CDialog
{
	DECLARE_DYNAMIC(CMembershipDlg)

public:
	CMembershipDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CMembershipDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MEMBERSHIP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_MembershipID;
	CString m_MembershipPassword;
public:
	std::map<CString, SOCKET_INFO>::iterator iter_c_info;
	bool Check_IdExist();
	int Check_IdString();
	bool Send_JoinMembership();
	int Check_Password();
	
	bool SendMembershipJoin();

	afx_msg void OnEnChangeEditMembershipId();
	CString m_static = _T("ป๓ลย : ");
	bool checkFirst = true;
	bool checkIdSuccess = false;
	afx_msg void OnBnClickedButtonMembershipJoin();

	CCLIENT_CONTROL* clnt_con;
};


