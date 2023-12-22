#pragma once
#include "tcp/Client_TCP.h"

#include <list>

// CChattingRoom dialog

struct FILE_INFO {
	CString file_name;
	char* file_data;
	long file_size;
};

struct BLOCK_INFO {
	WCHAR MessageType = 0;
	long size;
	char* block;
};

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

	CWinThread* m_sessionIdListThread;
	CWinThread* m_chattingThread;
	bool checksessionIdListThreadStart = false;
	bool checkchattingThreadStart = false;

	void SetInitialize_CLIENT_INFO_CHATTINGROOM(std::map<CString, SOCKET_INFO>::iterator iter);
	afx_msg void OnBnClickedButtonSendMessage();
	CString m_message;
	WCHAR* SetSendMessage(CString message, int& len, SOCKET_INFO& socket_info);
	CRichEditCtrl m_richEdit;
	bool Get_Parcing_Message(WCHAR* data, int len,CString &str);

	CEdit m_editControl;
	afx_msg void OnDropFiles(HDROP hDropInfo);

	// 2023.11.10 추가
	CRect richEditRect;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEnChangeRichedit21();
	afx_msg void OnDropfilesRichedit21(NMHDR* pNMHDR, LRESULT* pResult);
	//afx_msg LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnDragdropdoneRichedit21(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// 2023.11.13 추가
	std::list<BLOCK_INFO> Get_FileUpload_Completed(std::list<FILE_INFO> &f_list, SOCKET_INFO& socket_info);
	std::list<BLOCK_INFO> Get_FileUpload_Completed_Ver2(std::list<FILE_INFO> &f_list, SOCKET_INFO& socket_info);
	
	// 2023.12.06 파일 업로드 헤더 파트
	// message info
	WCHAR* Get_FileUpload_Completed_MessageInfo(WCHAR m_type,int &messageInfoLength,WCHAR* id,short idLength);
	WCHAR* Get_FileUpload_Completed_FileInfo(int& fileInfoLength, std::list<CString>* extension_list,
		std::list<CString>& fileNameWithoutExtension_list,
		int& extension_length, std::list<FILE_INFO>& f_list);
	std::list<CString> Get_FileUpload_Completed_FileInfo_extension_list(std::list<FILE_INFO>& f_list);
	std::list<CString> Get_FileUpload_Completed_FileInfo_fileNameWithOutExtension_list(std::list<FILE_INFO>& f_list);
	int Get_FileUpload_Completed_FileInfo_TotalFileSize(std::list<FILE_INFO>& f_list,int& oddNumberCount);
	int Get_FileUpload_Completed_FileInfo_FileNameLength(std::list<FILE_INFO>& f_list);
	std::list<int> Get_FileUpload_Completed_FileInfo_FileNameLength_list(std::list<FILE_INFO>& f_list);

	// file info

	// --

	// 2023.11.16 블록 단위 전송 기능 추가
	//std::list<char*> block_list;
	byte Get_Message_BlockCount(WCHAR* data, int& len,
		int sendIdLength, int messageInfoLength);
	
	std::list<BLOCK_INFO> Set_Message_Block(char* data, int& len,
		int sendIdLength, int messageInfoLength);
};

#include <Richedit.h>

class CMyRichEditControl : public CRichEditCtrl
{
	DECLARE_DYNAMIC(CMyRichEditControl)
public:
	CMyRichEditControl();
	virtual ~CMyRichEditControl();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnQueryAcceptData(WPARAM wParam, LPARAM lParam);
};