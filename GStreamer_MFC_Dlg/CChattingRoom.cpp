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
	
	clnt_con = new CCLIENT_CONTROL;
}

CChattingRoom::~CChattingRoom()
{
}

void CChattingRoom::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LIST_LOGIN_SESSION_ID, m_loginSessionList);
	DDX_Control(pDX, IDC_LIST_IDLIST, m_sessionIdList);
}


BEGIN_MESSAGE_MAP(CChattingRoom, CDialog)
	ON_WM_CLOSE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CChattingRoom::OnLvnItemchangedList1)
END_MESSAGE_MAP()


// CChattingRoom message handlers

void CChattingRoom::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	// 채팅창 종료 시, 로그아웃 할 지 여부를 묻는다.
	if (MessageBox(_T("로그아웃 하시겠습니까?"), _T("로그아웃"), MB_YESNO) == IDYES) {
		SuspendThread(this->m_pThread);
		this->checkThreadStart = false;
		
		WCHAR type = 'O';

		auto iter_c_info = theApp.socket.socket_map.find(theApp.socket.mapKey);

		CString data = _T("");
		CString id = _T("id ");
		CString idData(iter_c_info->second.ID);
		int length = id.GetLength() + _tcslen(iter_c_info->second.ID);

		data += id + idData;
		
		if (clnt_con->SendData(type, data.GetBuffer(), data.GetLength(), iter_c_info->second)) {
			// 추후에 이곳에 기존에 있던 채팅 내용들 같은 경우를 작성해야함.

			::SendMessage(this->m_hWnd, WM_DESTROY, NULL, NULL);
		}
	}
	CDialog::OnClose();
}

void CChattingRoom::Set_ConnectSessionIdList()
{
	WCHAR type = 'C';

	auto iter_c_info = theApp.socket.socket_map.find(theApp.socket.mapKey);

	UpdateData(TRUE);

	if(!this->m_sessionIdList){
		return;
	}

	if (clnt_con->SendData(type, NULL, 0, iter_c_info->second)) {
		// 추후에 이곳에 기존에 있던 채팅 내용들 같은 경우를 작성해야함.
		while (true) {
			auto iter_c_info = theApp.socket.socket_map.find(theApp.socket.mapKey);

			if (iter_c_info->second.last_type == 'S') {
				// 성공
				std::list<CString> list = Get_IdList(iter_c_info->second.sendData);

				if (list.size() > 0)
				{
					std::list<CString>::iterator iter = list.begin();
					CString str = _T("");
					for (int i = 0; i < list.size(), iter != list.end(); i++,iter++) {
						//LPCTSTR k = iter->GetBuffer();
						str = iter->GetString();
						int k = this->m_sessionIdList.GetItemCount();
						this->m_sessionIdList.InsertItem(this->m_sessionIdList.GetItemCount(), str);
					}
				}
				break;
			}
		}
	}
}

void CChattingRoom::Free_ColumnDataList(std::list<CCLIENT_CONTROL::COLUMN_DATA>& list)
{
	for (auto iter = list.begin(); iter != list.end();iter++) {
		if (iter->len > 0)
			free(iter->data);
	}
}

std::list<CString> CChattingRoom::Get_IdList(WCHAR* data)
{
	int pos = 0;

	pos++;

	int totalLength = 0;
	memcpy(&totalLength,&data[pos], sizeof(int));
	pos += sizeof(int);
	// Column Name List
	short columnNameCount = 0;
	memcpy(&columnNameCount, &data[pos], sizeof(short));
	pos += sizeof(short);

	std::list<CCLIENT_CONTROL::COLUMN_DATA> columnNameList;

	for (int i = 0; i < (int)columnNameCount; i++) {
		CCLIENT_CONTROL::COLUMN_DATA tempData;
		memcpy(&tempData.db_type, &data[pos], sizeof(byte));
		pos += sizeof(byte);
		memcpy(&tempData.len, &data[pos], sizeof(int));
		pos += sizeof(int);
		//tempData.data = (WCHAR*)malloc(tempData.len);
		//memset(tempData.data,0,tempData.len*2);
		memcpy(tempData.data, &data[pos], tempData.len*2);
		tempData.data[tempData.len] = 0;
		pos += tempData.len;
		columnNameList.push_back(tempData);
	}

	short columnValueCount = 0;
	memcpy(&columnValueCount, &data[pos], sizeof(short));
	pos += sizeof(short);

	//std::list<CCLIENT_CONTROL::COLUMN_DATA> columnValueList;

	std::list<CString> IdList;

	for (int i = 0; i < (int)columnValueCount; i++) {
		CCLIENT_CONTROL::COLUMN_DATA tempData;
		memcpy(&tempData.len, &data[pos], sizeof(int));
		pos += sizeof(int);
		//tempData.data = (WCHAR*)malloc(tempData.len);
		//memset(tempData.data, 0, tempData.len*2);
		memcpy(tempData.data, &data[pos], tempData.len*2);
		tempData.data[tempData.len] = 0;
		pos += tempData.len;
		
		CString str = tempData.data;
		IdList.push_back(str);
	}

	//this->Free_ColumnDataList(columnNameList);

	return IdList;
}

void CChattingRoom::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

UINT ChattingRoomThread(LPVOID param);

BOOL CChattingRoom::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	//Set_ConnectSessionIdList();

	checkThreadStart = true;
	this->m_pThread = AfxBeginThread(ChattingRoomThread, this);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CChattingRoom::SetInitialize_CLIENT_INFO_CHATTINGROOM(std::map<CString, SOCKET_INFO>::iterator iter)
{
	iter->second.last_type = 0;
	iter->second.last_type_success = -1;
	if (iter->second.sendDataLength > 0) {
		wmemset(iter->second.sendData, 0, 1024);
		iter->second.sendDataLength = 0;
	}
}

UINT ChattingRoomThread(LPVOID param) {
	CChattingRoom* pMain = (CChattingRoom*)param;

	int checkSend = false;

	while (pMain->checkThreadStart) {
		Sleep(100);

		auto iter_c_info = theApp.socket.socket_map.find(theApp.socket.mapKey);

		// 2023.10.23
		// 클라이언트측에서 서버에게 유저 정보를 요청한다.
		if (checkSend == false) {
			WCHAR type = 'C';
			if (pMain->clnt_con->SendData(type, NULL, 0, iter_c_info->second))
			{
				checkSend = true;
			}
		}

		if (iter_c_info->second.last_type == 'S' && checkSend == true
			&& pMain->checkThreadStart == true) {
			// 서버측에서 접속 중인 유저 변화가 생겼다.
			std::list<CString> list = pMain->Get_IdList(iter_c_info->second.sendData);

			if (list.size() > 0)
			{
				std::list<CString>::iterator iter = list.begin();
				pMain->m_sessionIdList.DeleteAllItems();
				CString str = _T("");
				for (int i = 0; i < list.size(), iter != list.end(); i++, iter++) {
					//LPCTSTR k = iter->GetBuffer();
					str = iter->GetString();
					
					// 아이디 리스트 중에 현재 내 아이디 옆엔 XXX '(나)' <- 이걸 붙여주자.
					if (str.Compare(iter_c_info->second.ID) == 0) {
						// 찾았다.
						str += " (나)";
					}

					int k = pMain->m_sessionIdList.GetItemCount();
					pMain->m_sessionIdList.InsertItem(pMain->m_sessionIdList.GetItemCount(), str);
				}
			}
			checkSend = false;
		}

		// 마무리..
		//SOCKET_INFO* pTemp = &(*iter_c_info);
//		pMain->clnt_con->SetInitialize_CLIENT_INFO(iter_c_info);

		// 2023.10.20 발생한 이슈. 이곳에서 CChattingRoom 포인터에서 멤버 변수로 있는
		// CCLIENT_CONTROL* 변수에서 CCLIENT_CONTROL가 상속받은 CTCP_SOCKET 클래스 내부의 
		// 멤버 함수인 SetInitialize_CLIENT_INFO() 중 iterator을 인자로 받는 함수가 링크가
		// 안맞는다고 에러가 발생한다. 해결이 안되어 별도의 같은 멤버 변수를 추가하였다.
		pMain->SetInitialize_CLIENT_INFO_CHATTINGROOM(iter_c_info);
	}

	return 0;
}