#pragma once
#include "tcp/Client_TCP.h"

#include <list>

// CChattingRoom dialog

class CChattingRoom : public CDialog
{
	DECLARE_DYNAMIC(CChattingRoom)

public:
	CChattingRoom(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CChattingRoom();
public:
	// X초 주기로 서버와 통신하여 현재 접속 중인 유저 ID들의 집합을 가져와 화면에 현시한다.
	void Set_ConnectSessionIdList();
	std::list<CString> Get_IdList(WCHAR* data);
	void Free_ColumnDataList(std::list<CCLIENT_CONTROL::COLUMN_DATA>& list);
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CHATTING_ROOM };
	//enum { IDD = IDD_DIALOG1};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
//	CListBox m_loginSessionList;

	CCLIENT_CONTROL* clnt_con;
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	CListCtrl m_sessionIdList;
	virtual BOOL OnInitDialog();

	CWinThread* m_pThread;
	bool checkThreadStart = false;
	void SetInitialize_CLIENT_INFO_CHATTINGROOM(std::map<CString, SOCKET_INFO>::iterator iter);
};
