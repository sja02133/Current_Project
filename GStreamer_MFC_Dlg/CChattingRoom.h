#pragma once


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
	CListBox m_loginSessionList;
};
