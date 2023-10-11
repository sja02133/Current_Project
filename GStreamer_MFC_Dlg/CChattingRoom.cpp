// CChattingRoom.cpp : implementation file
//

#include "pch.h"
#include "GStreamer_MFC_Dlg.h"
#include "CChattingRoom.h"
#include "afxdialogex.h"


// CChattingRoom dialog

IMPLEMENT_DYNAMIC(CChattingRoom, CDialog)

CChattingRoom::CChattingRoom(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_CHATTING_ROOM, pParent)
	//: CDialog(IDD_DIALOG1, pParent)
{
	AfxInitRichEdit();
	Set_ConnectSessionIdList();
}

CChattingRoom::~CChattingRoom()
{
}

void CChattingRoom::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOGIN_SESSION_ID, m_loginSessionList);
}


BEGIN_MESSAGE_MAP(CChattingRoom, CDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CChattingRoom message handlers

void CChattingRoom::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	// ä��â ���� ��, �α׾ƿ� �� �� ���θ� ���´�.
	if (MessageBox(_T("�α׾ƿ� �Ͻðڽ��ϱ�?"), _T("�α׾ƿ�"), MB_YESNO) == IDYES) {
		WCHAR type = 'O';

		auto iter_c_info = theApp.socket.client_map.find(theApp.socket.mapKey);

		CString data = _T("");
		CString id = _T("id ");
		CString idData(iter_c_info->second.ID);
		int length = id.GetLength() + _tcslen(iter_c_info->second.ID);

		data += id + idData;

		if (theApp.socket.SendData(type, data.GetBuffer(), data.GetLength(), &theApp.socket)) {
			// ���Ŀ� �̰��� ������ �ִ� ä�� ����� ���� ��츦 �ۼ��ؾ���.

			::SendMessage(this->m_hWnd, WM_DESTROY, NULL, NULL);
		}
	}
	CDialog::OnClose();
}

void CChattingRoom::Set_ConnectSessionIdList()
{
	WCHAR type = 'C';

	auto iter_c_info = theApp.socket.client_map.find(theApp.socket.mapKey);

	if (theApp.socket.SendData(type, NULL, 0, &theApp.socket)) {
		// ���Ŀ� �̰��� ������ �ִ� ä�� ����� ���� ��츦 �ۼ��ؾ���.
		while (true) {
			auto iter_c_info = theApp.socket.client_map.find(theApp.socket.mapKey);

			if (iter_c_info->second.last_type == 'S') {
				// ����
				int kk = 0;
				
				break;
			}
		}
	}
}