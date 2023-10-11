#pragma once


// CChattingRoom dialog

class CChattingRoom : public CDialog
{
	DECLARE_DYNAMIC(CChattingRoom)

public:
	CChattingRoom(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CChattingRoom();
public:
	// X�� �ֱ�� ������ ����Ͽ� ���� ���� ���� ���� ID���� ������ ������ ȭ�鿡 �����Ѵ�.
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
