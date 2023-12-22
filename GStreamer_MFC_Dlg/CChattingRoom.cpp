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
	, m_message(_T(""))
{
	AfxInitRichEdit();
	
	clnt_con = new CCLIENT_CONTROL;

	TRACE(_T("CChattingRoom() DONE"));
}

CChattingRoom::~CChattingRoom()
{
}

void CChattingRoom::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LIST_LOGIN_SESSION_ID, m_loginSessionList);
	DDX_Control(pDX, IDC_LIST_IDLIST, m_sessionIdList);
	DDX_Text(pDX, IDC_EDIT2, m_message);
	DDX_Control(pDX, IDC_RICHEDIT21, m_richEdit);
	DDX_Control(pDX, IDC_EDIT2, m_editControl);
}

BEGIN_MESSAGE_MAP(CChattingRoom, CDialog)
	ON_WM_CLOSE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CChattingRoom::OnLvnItemchangedList1)
	ON_BN_CLICKED(IDC_BUTTON_SEND_MESSAGE, &CChattingRoom::OnBnClickedButtonSendMessage)
	ON_WM_DROPFILES()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_EN_CHANGE(IDC_RICHEDIT21, &CChattingRoom::OnEnChangeRichedit21)
	ON_NOTIFY(EN_DROPFILES, IDC_RICHEDIT21, &CChattingRoom::OnDropfilesRichedit21)
	//ON_WM_LBUTTONDOWN(IDC_RICHEDIT21, &CChattingRoom::WindowProc)
	ON_NOTIFY(EN_DRAGDROPDONE, IDC_RICHEDIT21, &CChattingRoom::OnDragdropdoneRichedit21)
END_MESSAGE_MAP()


// CChattingRoom message handlers

//LRESULT CChattingRoom::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//#ifdef _DEBUG
//	if (message == WM_COMMAND)
//		if (HIWORD(wParam) == BN_CLICKED)
//		{
//			CString strCtrlID;
//			strCtrlID.Format(_T("CtrlID: %d\r\n"), LOWORD(wParam));
//			TRACE(strCtrlID);
//		}
//#endif
//	return CChattingRoom::WindowProc(message, wParam, lParam);
//}

void CChattingRoom::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	// 채팅창 종료 시, 로그아웃 할 지 여부를 묻는다.

	int select = MessageBox(_T("로그아웃 하시겠습니까?"), _T("로그아웃"), MB_YESNO);

	if (select == IDYES) {
		SuspendThread(this->m_sessionIdListThread);
		this->checksessionIdListThreadStart = false;

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
	else if (select == IDNO) {
		return;
	}
	else
		return;
	
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
UINT ChattingEditControlUpdate(LPVOID param);

BOOL CChattingRoom::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	//Set_ConnectSessionIdList();

	checksessionIdListThreadStart = true;
	checkchattingThreadStart = true;

	this->m_sessionIdListThread = AfxBeginThread(ChattingRoomThread, this);
	this->m_chattingThread = AfxBeginThread(ChattingEditControlUpdate, this);

	// 2023.11.10 추가
	// rich edit의 위치를 알아야 한다. 만약 dialog의 위치가 변경되는 경우에도 다시 계산해야하긴 한다.
	m_richEdit.GetWindowRect(&richEditRect);
	ScreenToClient(&richEditRect);

	::DragAcceptFiles(this->m_editControl.m_hWnd, true);

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
void TextOut_custom(CString str, COLORREF textColor, CRichEditCtrl& richEdit);
UINT ChattingEditControlUpdate(LPVOID param) {
	CChattingRoom* pMain = (CChattingRoom*)param;

	int checkSend = false;

	while (pMain->checkchattingThreadStart) {
		//TRACE("ChattingEditControlUpdate() -> while\n");
		Sleep(1000);

		auto iter_c_info = theApp.socket.socket_map.find(theApp.socket.mapKey);

		if (iter_c_info->second.last_type == 'P' && pMain->checkchattingThreadStart == true) {
			if (iter_c_info->second.sendData[0] == '\0' || iter_c_info->second.sendDataLength == 0)
				continue;
			
			TRACE("ChattingEditControlUpdate() -> while -> Get_Parcing_Message() before\n");
			
			CString cur_message = _T("");
			pMain->m_richEdit.GetWindowTextW(cur_message);
			pMain->Get_Parcing_Message(iter_c_info->second.sendData, iter_c_info->second.sendDataLength, cur_message);
			TRACE("ChattingEditControlUpdate() -> while -> Get_Parcing_Message() after\n");
			//pMain->m_richEdit.SetWindowTextW(cur_message);

			TextOut_custom(cur_message, RGB(0, 0, 0), pMain->m_richEdit);

			pMain->SetInitialize_CLIENT_INFO_CHATTINGROOM(iter_c_info);
		}
	}
	return 1;
}

void TextOut_custom(CString str,COLORREF textColor, CRichEditCtrl &richEdit) {
	int first_pos = richEdit.LineIndex(richEdit.GetLineCount());
	richEdit.SetSel(first_pos, first_pos);
	CPoint point;
	point = richEdit.PosFromChar(first_pos);
	richEdit.SetCaretPos(point);
	richEdit.SetFocus();

	// 문자출력
	CHARFORMAT cf;
	memset(&cf, 0, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR;
	cf.crTextColor = textColor;

	richEdit.SetSelectionCharFormat(cf);

	richEdit.SetWindowTextW(_T(""));

	richEdit.ReplaceSel(str);
}

bool CChattingRoom::Get_Parcing_Message(WCHAR* data, int len,CString &str)
{
	//CString str = _T("");

	if(!str.IsEmpty())
		str += _T("\n");

	int pos = 0;
	pos++;	// pass to type

	int totalLength = 0;
	memcpy(&totalLength, &data[pos], sizeof(int));
	pos += sizeof(int);

	//message info....
	int messageInfoLength = 0;
	memcpy(&messageInfoLength, &data[pos], sizeof(int));
	pos += sizeof(int);

	WCHAR type = 0;
	memcpy(&type, &data[pos], 1);
	pos += sizeof(BYTE);

	short sendIdLength = 0;
	memcpy(&sendIdLength, &data[pos], sizeof(short));
	pos += sizeof(short);

	WCHAR sendId[100] = { 0, };
	memcpy(&sendId[0], &data[pos], sendIdLength*2);
	pos += sendIdLength;

	str += sendId;

	str += _T(" : ");

	// message...
	int messageLength = 0;
	memcpy(&messageLength, &data[pos], sizeof(int));
	pos += sizeof(int);

	//WCHAR* message = 0;
	//message = (WCHAR*)malloc(messageLength);
	//memset(message, 0, messageLength * 2);
	//memcpy(&message[0], &data[pos], messageLength);

	WCHAR message[1000] = { 0, };
	memcpy(&message[0], &data[pos], messageLength*2);	// WCHAR 에 따른 length * 2

	for (int i = 0; i < messageLength; i++) {
		str += message[i];
	}

	//return str;
	return true;
}

UINT ChattingRoomThread(LPVOID param) {
	CChattingRoom* pMain = (CChattingRoom*)param;

	int checkSend = false;

	while (pMain->checksessionIdListThreadStart) {
		Sleep(1000);

		auto iter_c_info = theApp.socket.socket_map.find(theApp.socket.mapKey);

		// 2023.10.23
		// 클라이언트측에서 서버에게 유저 정보를 요청한다.
		if (checkSend == false && pMain->checksessionIdListThreadStart) {
			WCHAR type = 'C';
			if (pMain->clnt_con->SendData(type, NULL, 0, iter_c_info->second))
			{
				checkSend = true;
			}
		}

		if (iter_c_info->second.last_type == 'S' && checkSend == true
			&& pMain->checksessionIdListThreadStart == true) {
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
			pMain->SetInitialize_CLIENT_INFO_CHATTINGROOM(iter_c_info);
		}

		// 마무리..
		//SOCKET_INFO* pTemp = &(*iter_c_info);
//		pMain->clnt_con->SetInitialize_CLIENT_INFO(iter_c_info);

		// 2023.10.20 발생한 이슈. 이곳에서 CChattingRoom 포인터에서 멤버 변수로 있는
		// CCLIENT_CONTROL* 변수에서 CCLIENT_CONTROL가 상속받은 CTCP_SOCKET 클래스 내부의 
		// 멤버 함수인 SetInitialize_CLIENT_INFO() 중 iterator을 인자로 받는 함수가 링크가
		// 안맞는다고 에러가 발생한다. 해결이 안되어 별도의 같은 멤버 변수를 추가하였다.
	}

	return 0;
}

std::list<BLOCK_INFO> CChattingRoom::Get_FileUpload_Completed(std::list<FILE_INFO>& f_list, SOCKET_INFO& socket_info)
{
	std::list<BLOCK_INFO> block_list;

	// type(1) + totalLength(4) + Message Info (4 + 1 + 2 + 가변 + 00 + 1 + 가변) + Data (8 + 가변)

	// 2023.11.16 기능 변경
	// MTU에 따라 block 단위로 나눠서 보내야한다.

	std::list<CString> extension_list;

	int totalLength = 0;
	int dataPositionSize = 0;

	std::list<int> fileNameLength_list;
	int fileNameLength = 0;
	int totalFileSize = 0;
	for (auto iter = f_list.begin(); iter != f_list.end(); iter++) {
		fileNameLength += iter->file_name.GetLength();
		fileNameLength_list.push_back(iter->file_name.GetLength());
		int r_pos = iter->file_name.ReverseFind(_T('.'));
		CString extension = iter->file_name.Mid(r_pos+1, iter->file_name.GetLength());
		extension_list.push_back(extension);
		totalFileSize += iter->file_size;
	}
	totalLength += 1;									// type
	totalLength += sizeof(int);							// totalLength
	// Message Info
	int messageInfoLength = 0;
	messageInfoLength += sizeof(int);							// message info length
	messageInfoLength += sizeof(BYTE);						// message info type
	messageInfoLength += sizeof(short);						// send id length
	messageInfoLength += _tcslen(socket_info.ID);				// send ID
	messageInfoLength += sizeof(BYTE);						// block count
	messageInfoLength += sizeof(BYTE);						// file count
	int extension_length = 0;
	for (auto iter = extension_list.begin(); iter != extension_list.end(); iter++) {
		extension_length += iter->GetLength();
		extension_length += 1;	// 분리 00 자리
	}													// 확장자 명
	messageInfoLength += extension_length;
	messageInfoLength += (sizeof(short) * f_list.size()) + (fileNameLength * 2);
	// 설명1 => fileNameLength * 2 하는 이유는 현재 fileNameLength은 WCHAR 기준 길이가 아닌 실제 길이이기때문이다.

	totalLength += messageInfoLength;
	// 파일 내부의 데이터의 크기는 별도로 알아내서 나중에 char* 데이터형에 (totalLength * 2 + file size) 만큼 malloc한다.
	
	WCHAR* sendMessage = (WCHAR*)malloc(totalLength);
	memset(sendMessage, 0, totalLength);
	WCHAR type = 'P';

	int pos = 0;

	memcpy(&sendMessage[pos], &type, sizeof(WCHAR));

	pos++;

	// 추가적으로 뒤의 데이터 부의 영역의 자리를 계산하지 않았음으로, 나중에 이 위치는 따로 계산한다.
	//memcpy(&sendMessage[pos], &totalLength, sizeof(int));
	pos += sizeof(int);

	// setting Message Info...
	// MTU를 나눠야 하기 때문에 이곳에서 한번 정리해야한다.
	// MTU 값은 1500이므로, header 부분 ( Type + total length + Message Info ) 값을 먼저 빼고 나머지 값에서 계산한다.
	memcpy(&sendMessage[pos], &messageInfoLength, sizeof(int));
	pos += sizeof(int);
	WCHAR m_type = 'F';	// Message Info 에서의 Type 'M'은 파일 타입이다. 추가적인 타입은 추후 별도로 정해야 한다.
	memcpy(&sendMessage[pos], &m_type, sizeof(WCHAR));
	pos += 1;
	short id_length = _tcslen(socket_info.ID);
	memcpy(&sendMessage[pos], &id_length, sizeof(short));
	pos += sizeof(short);
	memcpy(&sendMessage[pos], socket_info.ID, _tcslen(socket_info.ID) * 2);
	pos += id_length;
	byte blockCount = 0;
	memcpy(&sendMessage[pos], &blockCount, sizeof(byte));
	pos += sizeof(byte);
	byte file_count = f_list.size();
	memcpy(&sendMessage[pos], &file_count, sizeof(byte));
	pos += sizeof(byte);
	
	for (auto iter = extension_list.begin(); iter != extension_list.end(); iter++) {
		memcpy(&sendMessage[pos], iter->GetBuffer(),iter->GetLength()*2);
		pos += iter->GetLength();
		sendMessage[pos] = _T('\0');
		pos += 1;
	}

	for (auto iter = f_list.begin(); iter != f_list.end(); iter++) {
		short fileNameLength_short = iter->file_name.GetLength();
		memcpy(&sendMessage[pos], &fileNameLength_short, sizeof(short));
		pos += sizeof(short);
		memcpy(&sendMessage[pos], iter->file_name.GetBuffer(), fileNameLength_short * 2);
		pos += fileNameLength_short;
	}

	if (pos != totalLength) {
		int kkk = 0;
	}

	// setting Data
	int resultTotalSize = (totalLength*2) + ((sizeof(long)*2)*file_count) + totalFileSize;
	// 설명1 : (totalLength*2) => WCHAR* 로 위에서 malloc 하였기 때문에 char* 형에서는 WCHAR 자릿수에 맞추어 *2를 해줘야 한다.
	// 설명2 : ((sizeof(long)*2)*file_count) => (sizeof(long)*2)은 WCHAR 자릿수에 맞추어 *2를 해줬다.
	char* result = (char*)malloc(resultTotalSize);
	memset(result, 0, resultTotalSize);
	memcpy(&result[0], &sendMessage[0], pos*2);
	//pos *= 2;

	for (auto iter = f_list.begin(); iter != f_list.end(); iter++) {
		memcpy(&result[pos], &iter->file_size, sizeof(long));
		pos += sizeof(long);
		memcpy(&result[pos], iter->file_data, iter->file_size);
		pos += iter->file_size;
	}

	memcpy(&result[1], &pos, sizeof(int));
	memcpy(&result[5], &messageInfoLength, sizeof(int));

	if (pos <= MTU_MAX) {
		// block count 위치를 가서 block count를 1로 변경한다.
		int blockPos = 0;
		blockPos += (1 + 4 + 4 + 1 + 2 + id_length);
		blockCount = 1;
		memcpy(&result[blockPos], &blockCount, sizeof(byte));
		BLOCK_INFO b_info;
		b_info.MessageType = _T('F');
		b_info.size = pos;
		b_info.block = (char*)malloc(pos);
		memcpy(b_info.block, result, pos);
		block_list.push_back(b_info);
	}
	else if(pos > MTU_MAX){
		// 블록 단위로 나눠서 보내야한다. 그렇기에 block 계산을 해줘야 한다.
		// 블록 계산을 해보자.
		block_list = Set_Message_Block(result, resultTotalSize, _tcslen(socket_info.ID), messageInfoLength);
	}
	else {
		// something error??
	}

	free(sendMessage);
	free(result);

	return block_list;
}

std::list<BLOCK_INFO> CChattingRoom::Get_FileUpload_Completed_Ver2(std::list<FILE_INFO>& f_list, SOCKET_INFO& socket_info)
{
	std::list<BLOCK_INFO> block_list;

	// type(1) + totalLength(4) + Message Info (4 + 1 + 2 + 가변 + 00 + 1 + 가변) + Data (8 + 가변)
	
	int totalLength = 0;
	int dataPositionSize = 0;

	std::list<int> fileNameLength_list;
	int fileNameLength = 0;
	int totalFileSize = 0;
	std::list<CString> extension_list;
	std::list<CString> fileNameWithoutExtension_list;

	// 홀수 갯수!
	int oddNumberCount = 0;	// 파일 사이즈가 홀수인 갯수.

	for (auto iter = f_list.begin(); iter != f_list.end(); iter++) {
		// 확장자 명 저장..
		int r_pos = iter->file_name.ReverseFind(_T('.'));
		CString extension = iter->file_name.Mid(r_pos + 1, iter->file_name.GetLength());
		extension_list.push_back(extension);
		
		// 파일 이름 저장 (확장자 제외)
		int slash_pos = iter->file_name.ReverseFind(_T('\\'));
		CString fileName = iter->file_name.Mid(slash_pos+1, r_pos);
		r_pos = fileName.ReverseFind(_T('.'));
		fileName = fileName.Mid(0, r_pos);
		fileNameWithoutExtension_list.push_back(fileName);

		// 파일 이름 길이
		fileNameLength += fileName.GetLength();
		fileNameLength_list.push_back(fileName.GetLength());
		
		// 총 파일 크기
		totalFileSize += iter->file_size;

		// 파일 사이즈가 홀수면?
		if (iter->file_size % 2 != 0) {
			oddNumberCount++;
		}
	}

	totalLength += 1;									// type
	totalLength += sizeof(int);							// totalLength

	// Message Info start..
	//int messageInfoLength = 0;
	//messageInfoLength += sizeof(int);							// message info length
	//messageInfoLength += sizeof(BYTE);						// message info type
	//messageInfoLength += sizeof(short);						// send id length
	//messageInfoLength += _tcslen(socket_info.ID);				// send ID
	//messageInfoLength += sizeof(BYTE);						// block count
	//// message Info 부분만..
	//WCHAR* part_messageInfo = (WCHAR*)malloc(messageInfoLength*2);
	//memset(part_messageInfo, 0, messageInfoLength*2);
	//int mif_pos = 0;	//message info pos
	//memcpy(&part_messageInfo[mif_pos], &messageInfoLength, sizeof(int));
	//mif_pos += sizeof(int);
	//WCHAR m_type = 'F';	// Message Info 에서의 Type 'M'은 파일 타입이다. 추가적인 타입은 추후 별도로 정해야 한다.
	//memcpy(&part_messageInfo[mif_pos], &m_type, sizeof(WCHAR));
	//mif_pos += 1;
	//short id_length = _tcslen(socket_info.ID);
	//memcpy(&part_messageInfo[mif_pos], &id_length, sizeof(short));
	//mif_pos += sizeof(short);
	//memcpy(&part_messageInfo[mif_pos], socket_info.ID, _tcslen(socket_info.ID) * 2);
	//mif_pos += id_length;
	//byte blockCount = 0;
	//memcpy(&part_messageInfo[mif_pos], &blockCount, sizeof(byte));
	//mif_pos += sizeof(byte);
	//if (mif_pos != messageInfoLength)
	//	int bbb = 0;
	// message info done...
	int messageInfoLength = 0;
	int blockCount = 0;
	WCHAR* part_messageInfo = Get_FileUpload_Completed_MessageInfo(_T('F'), messageInfoLength, socket_info.ID, _tcslen(socket_info.ID));
	

	//// file info start
	//int fileInfoLength = 0;
	//fileInfoLength += sizeof(BYTE);						// file count
	//int extension_length = 0;
	//for (auto iter = extension_list.begin(); iter != extension_list.end(); iter++) {
	//	extension_length += iter->GetLength();
	//	extension_length += 1;	// 분리 00 자리
	//}													// 확장자 명
	//fileInfoLength += extension_length;
	//fileInfoLength += ((sizeof(short)*f_list.size()) + (fileNameLength));
	//// 설명1 => fileNameLength * 2 하는 이유는 현재 fileNameLength은 WCHAR 기준 길이가 아닌 실제 길이이기때문이다.

	//WCHAR* part_fileInfo = (WCHAR*)malloc(fileInfoLength*2);
	//memset(part_fileInfo, 0, fileInfoLength*2);
	//int fif_pos = 0;
	//byte file_count = f_list.size();
	//memcpy(&part_fileInfo[fif_pos], &file_count, sizeof(byte));
	//fif_pos += sizeof(byte);
	//for (auto iter = extension_list.begin(); iter != extension_list.end(); iter++) {
	//	memcpy(&part_fileInfo[fif_pos], iter->GetBuffer(), iter->GetLength() * 2);
	//	fif_pos += iter->GetLength();
	//	part_fileInfo[fif_pos] = _T('\0');
	//	fif_pos += 1;
	//}
	//for (auto iter = fileNameWithoutExtension_list.begin(); iter != fileNameWithoutExtension_list.end(); iter++) {
	//	short fileNameLength_short = iter->GetLength();
	//	memcpy(&part_fileInfo[fif_pos], &fileNameLength_short, sizeof(short));
	//	fif_pos += sizeof(short);
	//	memcpy(&part_fileInfo[fif_pos], iter->GetBuffer(), fileNameLength_short * 2);
	//	fif_pos += fileNameLength_short;
	//}
	//if (fif_pos != fileInfoLength)
	//	int aaa = 0;
	//// file info done

	int fileInfoLength = 0;
	int extension_length = 0;
	WCHAR* part_fileInfo = Get_FileUpload_Completed_FileInfo(fileInfoLength,
		&extension_list,fileNameWithoutExtension_list, extension_length,f_list);

	totalLength += (messageInfoLength + fileInfoLength);
	// 파일 내부의 데이터의 크기는 별도로 알아내서 나중에 char* 데이터형에 (totalLength * 2 + file size) 만큼 malloc한다.

	WCHAR* sendMessage = (WCHAR*)malloc(totalLength*2);
	memset(sendMessage, 0, totalLength*2);
	WCHAR type = 'P';

	int pos = 0;

	memcpy(&sendMessage[pos], &type, sizeof(WCHAR));
	pos++;

	// 추가적으로 뒤의 데이터 부의 영역의 자리를 계산하지 않았음으로, 나중에 이 위치는 따로 계산한다.
	//memcpy(&sendMessage[pos], &totalLength, sizeof(int));
	pos += sizeof(int);

	// setting Message Info...
	// MTU를 나눠야 하기 때문에 이곳에서 한번 정리해야한다.
	// MTU 값은 1500이므로, header 부분 ( Type + total length + Message Info ) 값을 먼저 빼고 나머지 값에서 계산한다.
	memcpy(&sendMessage[pos], &part_messageInfo[0], messageInfoLength *2);
	pos += messageInfoLength;
	free(part_messageInfo);

	memcpy(&sendMessage[pos], &part_fileInfo[0], fileInfoLength *2);
	pos += fileInfoLength;
	free(part_fileInfo);
	if (pos != totalLength) {
		int kkk = 0;
	}

	// file data start..
	char* file_data = 0;

	file_data = (char*)malloc((sizeof(long) * f_list.size()) + totalFileSize + oddNumberCount);
	memset(file_data, 0, (sizeof(long)* f_list.size()) + totalFileSize + oddNumberCount);
	int fdt_pos = 0;
	for (auto iter = f_list.begin(); iter != f_list.end(); iter++) {
		memcpy(&file_data[fdt_pos], &iter->file_size, sizeof(long));
		fdt_pos += sizeof(long);
		memcpy(&file_data[fdt_pos], iter->file_data, iter->file_size);
		fdt_pos += iter->file_size;
		if (iter->file_size % 2 != 0)
			fdt_pos++;
	}
	if (fdt_pos != (sizeof(long) * f_list.size()) + totalFileSize + oddNumberCount)
		int ddd = 0;
	// file data done..

	// setting Data
	int resultTotalSize = (totalLength * 2) + fdt_pos;
	// 설명1 : (totalLength*2) => WCHAR* 로 위에서 malloc 하였기 때문에 char* 형에서는 WCHAR 자릿수에 맞추어 *2를 해줘야 한다.
	// 설명2 : ((sizeof(long)*2)*file_count) => (sizeof(long)*2)은 WCHAR 자릿수에 맞추어 *2를 해줬다.
	char* result = (char*)malloc(resultTotalSize);
	memset(result, 0, resultTotalSize);
	int result_pos = 0;
	memcpy(&result[0], &sendMessage[0], totalLength * 2);
	free(sendMessage);
	result_pos += (totalLength * 2);

	memcpy(&result[result_pos], &file_data[0], fdt_pos);
	free(file_data);
	result_pos += fdt_pos;
	memcpy(&result[2], &result_pos, sizeof(int));
	messageInfoLength += fileInfoLength;
	memcpy(&result[10], &messageInfoLength, sizeof(int));

	if (result_pos <= MTU_MAX) {
		// block count 위치를 가서 block count를 1로 변경한다.
		int blockPos = 0;
		blockPos += (1 + 4 + 4 + 1 + 2 + _tcslen(socket_info.ID));
		blockCount = 1;
		memcpy(&result[blockPos], &blockCount, sizeof(byte));
		BLOCK_INFO b_info;
		b_info.MessageType = _T('F');
		b_info.size = result_pos;
		b_info.block = (char*)malloc(result_pos);
		memcpy(b_info.block, result, result_pos);
		block_list.push_back(b_info);
	}
	else if (result_pos > MTU_MAX) {
		// 블록 단위로 나눠서 보내야한다. 그렇기에 block 계산을 해줘야 한다.
		// 블록 계산을 해보자.
		block_list = Set_Message_Block(result, resultTotalSize, _tcslen(socket_info.ID), messageInfoLength);
	}
	else {
		// something error??
	}

	
	free(result);

	return block_list;
}

WCHAR* CChattingRoom::Get_FileUpload_Completed_MessageInfo(WCHAR m_type, int& messageInfoLength, WCHAR* id, short idLength)
{
	if (messageInfoLength != 0)
		messageInfoLength = 0;

	// Message Info start..
	messageInfoLength += sizeof(int);							// message info length
	messageInfoLength += sizeof(BYTE);						// message info type
	messageInfoLength += sizeof(short);						// send id length
	messageInfoLength += idLength;				// send ID
	messageInfoLength += sizeof(BYTE);						// block count
	// message Info 부분만..
	WCHAR* part_messageInfo = (WCHAR*)malloc(messageInfoLength * 2);
	memset(part_messageInfo, 0, messageInfoLength * 2);
	int mif_pos = 0;	//message info pos
	memcpy(&part_messageInfo[mif_pos], &messageInfoLength, sizeof(int));
	mif_pos += sizeof(int);
	//WCHAR m_type = 'F';	// Message Info 에서의 Type 'M'은 파일 타입이다. 추가적인 타입은 추후 별도로 정해야 한다.
	memcpy(&part_messageInfo[mif_pos], &m_type, sizeof(WCHAR));
	mif_pos += 1;
	short id_length = idLength;
	memcpy(&part_messageInfo[mif_pos], &id_length, sizeof(short));
	mif_pos += sizeof(short);
	memcpy(&part_messageInfo[mif_pos], id, idLength * 2);
	mif_pos += id_length;
	byte blockCount = 0;
	memcpy(&part_messageInfo[mif_pos], &blockCount, sizeof(byte));
	mif_pos += sizeof(byte);
	if (mif_pos != messageInfoLength)
		int bbb = 0;// error
	// message info done...

	return part_messageInfo;
}

WCHAR* CChattingRoom::Get_FileUpload_Completed_FileInfo(int& fileInfoLength,
	std::list<CString>* extension_list,
	std::list<CString>& fileNameWithoutExtension_list,
	int& extension_length,
	std::list<FILE_INFO>& f_list)
{
	// file info start
	int fileNameLength = Get_FileUpload_Completed_FileInfo_FileNameLength(f_list);
	fileInfoLength += sizeof(BYTE);						// file count
	for (auto iter = extension_list->begin(); iter != extension_list->end(); iter++) {
		extension_length += iter->GetLength();
		extension_length += 1;	// 분리 00 자리
	}													// 확장자 명
	fileInfoLength += extension_length;
	fileInfoLength += ((sizeof(short) * f_list.size()) + (fileNameLength));
	// 설명1 => fileNameLength * 2 하는 이유는 현재 fileNameLength은 WCHAR 기준 길이가 아닌 실제 길이이기때문이다.

	WCHAR* part_fileInfo = (WCHAR*)malloc(fileInfoLength * 2);
	memset(part_fileInfo, 0, fileInfoLength * 2);
	int fif_pos = 0;
	byte file_count = f_list.size();
	memcpy(&part_fileInfo[fif_pos], &file_count, sizeof(byte));
	fif_pos += sizeof(byte);
	for (auto iter = extension_list->begin(); iter != extension_list->end(); iter++) {
		memcpy(&part_fileInfo[fif_pos], iter->GetBuffer(), iter->GetLength() * 2);
		fif_pos += iter->GetLength();
		part_fileInfo[fif_pos] = _T('\0');
		fif_pos += 1;
	}
	for (auto iter = fileNameWithoutExtension_list.begin(); iter != fileNameWithoutExtension_list.end(); iter++) {
		short fileNameLength_short = iter->GetLength();
		memcpy(&part_fileInfo[fif_pos], &fileNameLength_short, sizeof(short));
		fif_pos += sizeof(short);
		memcpy(&part_fileInfo[fif_pos], iter->GetBuffer(), fileNameLength_short * 2);
		fif_pos += fileNameLength_short;
	}
	if (fif_pos != fileInfoLength)
		int aaa = 0;
	// file info done

	return part_fileInfo;
}

std::list<int> CChattingRoom::Get_FileUpload_Completed_FileInfo_FileNameLength_list(std::list<FILE_INFO>& f_list)
{
	std::list<int> fileNameLength_list;
	for (auto iter = f_list.begin(); iter != f_list.end(); iter++) {
		// 확장자 명 저장..
		int r_pos = iter->file_name.ReverseFind(_T('.'));
		// 파일 이름 저장 (확장자 제외)
		int slash_pos = iter->file_name.ReverseFind(_T('\\'));
		CString fileName = iter->file_name.Mid(slash_pos + 1, r_pos);
		r_pos = fileName.ReverseFind(_T('.'));
		fileName = fileName.Mid(0, r_pos);

		// 파일 이름 길이
		fileNameLength_list.push_back(fileName.GetLength());
	}
	return fileNameLength_list;
}

int CChattingRoom::Get_FileUpload_Completed_FileInfo_FileNameLength(std::list<FILE_INFO>& f_list)
{
	int fileNameLength = 0;
	std::list<CString> extension_list;
	std::list<CString> fileNameWithoutExtension_list;
	for (auto iter = f_list.begin(); iter != f_list.end(); iter++) {
		// 확장자 명 저장..
		int r_pos = iter->file_name.ReverseFind(_T('.'));

		// 파일 이름 저장 (확장자 제외)
		int slash_pos = iter->file_name.ReverseFind(_T('\\'));
		CString fileName = iter->file_name.Mid(slash_pos + 1, r_pos);
		r_pos = fileName.ReverseFind(_T('.'));
		fileName = fileName.Mid(0, r_pos);

		// 파일 이름 길이
		fileNameLength += fileName.GetLength();
	}
	return fileNameLength;
}

int CChattingRoom::Get_FileUpload_Completed_FileInfo_TotalFileSize(std::list<FILE_INFO>& f_list,int& oddNumberCount)
{
	int totalFileSize = 0;
	for (auto iter = f_list.begin(); iter != f_list.end(); iter++) {
		// 총 파일 크기
		totalFileSize += iter->file_size;

		// 파일 사이즈가 홀수면?
		if (iter->file_size % 2 != 0) {
			oddNumberCount++;
		}
	}
	return totalFileSize;
}

std::list<CString> CChattingRoom::Get_FileUpload_Completed_FileInfo_extension_list(std::list<FILE_INFO>& f_list)
{
	std::list<CString> fileNameWithoutExtension_list;
	// 파일 이름 저장 (확장자 제외)
	for (auto iter = f_list.begin(); iter != f_list.end(); iter++) {
		int slash_pos = iter->file_name.ReverseFind(_T('\\'));
		int r_pos = iter->file_name.ReverseFind(_T('.'));
		CString fileName = iter->file_name.Mid(slash_pos + 1, r_pos);
		r_pos = fileName.ReverseFind(_T('.'));
		fileName = fileName.Mid(0, r_pos);
		fileNameWithoutExtension_list.push_back(fileName);
	}
	return fileNameWithoutExtension_list;
}

std::list<CString> CChattingRoom::Get_FileUpload_Completed_FileInfo_fileNameWithOutExtension_list(std::list<FILE_INFO>& f_list)
{
	std::list<CString> extension_list;
	for (auto iter = f_list.begin(); iter != f_list.end(); iter++) {
		// 확장자 명 저장..
		int r_pos = iter->file_name.ReverseFind(_T('.'));
		CString extension = iter->file_name.Mid(r_pos + 1, iter->file_name.GetLength());
		extension_list.push_back(extension);
	}
	return extension_list;
}

std::list<BLOCK_INFO> CChattingRoom::Set_Message_Block(char* data, int& len, int sendIdLength, int messageInfoLength)
{
	std::list<BLOCK_INFO> b_info_list;
	
	bool checkFirst = true;
	int breakCount = 0;		// 현재 block 넘버로 사용가능!
	int forceBreakCount = 1000;
	bool checkBreak = 0;
	bool headerSizeOverlapMTU_MAX = false;
	int rememberPos = 0;
	WCHAR* sendID = (WCHAR*)malloc(sendIdLength);
	memset(sendID, 0, sendIdLength);
	int sendIDpos = 1 + 4 + 4 + 1 + 2;
	sendIDpos *= 2;
	memcpy(sendID, &data[sendIDpos], sendIdLength*2);
	while (1) {
		if (checkBreak || (breakCount == forceBreakCount))
			break;
		if (checkFirst == true) {
			BLOCK_INFO b_info;
			// header의 크기가 MTU를 넘어가는 경우를 상정해야한다.
			b_info.MessageType = _T('F');
			//memcpy(&messageInfoLength, &data[5], sizeof(int));
			if ((messageInfoLength) > MTU_MAX - 5) {
				// 만약 messageInfoLength가 MTU(1500)에서 type(1) + totalLength(4) => 5를 뺀 1495보다 크다면 multi header 처리를 해줘야 한다.
				// MULTI HEADER 처리..
				headerSizeOverlapMTU_MAX = true;
			}
			
			b_info.size = MTU_MAX;
			b_info.block = (char*)malloc(MTU_MAX);
			memcpy(&b_info.block, &data, MTU_MAX);
			rememberPos += b_info.size;
			b_info_list.push_back(b_info);
			checkFirst = false;
		}
		else {
			// 2번째 블록부터 만들어야 한다.
			BLOCK_INFO b_info;
			if (headerSizeOverlapMTU_MAX == true) {
				// 헤더의 크기가 MTU 최대값 1500을 넘어가는 경우가 있다.
				// 오버되는 값의 헤더 값은 헤더만 담아서 넘긴다. 
				b_info.MessageType = _T('H');
				if ((messageInfoLength + 5) - rememberPos > MTU_MAX) {
					// 만약 헤더값이 또 오버된다면?
					b_info.size = MTU_MAX;
					char* tempBlock = (char*)malloc(MTU_MAX);
					memset(tempBlock, 0, MTU_MAX);
					int pos = 0;
					WCHAR type = 'P';
					memcpy(&tempBlock[pos], &type, sizeof(WCHAR));
					pos += sizeof(WCHAR);
					int length = MTU_MAX;
					memcpy(&tempBlock[pos], &length, sizeof(int));
					pos += sizeof(int);
					memcpy(&tempBlock[pos], &b_info.MessageType, sizeof(WCHAR));
					pos += sizeof(WCHAR);
					short sendIdLength_short = (short)sendIdLength;
					memcpy(&tempBlock[pos], &sendIdLength_short, sizeof(short));
					pos += sizeof(short);
					memcpy(&tempBlock[pos], &sendID, sendIdLength * 2);
					pos += sendIdLength * 2;
					byte blockNum = (byte)breakCount + 1;
					memcpy(&tempBlock[pos], &blockNum, sizeof(byte));
					pos += sizeof(byte);
					// 현재 pos는 header의 크기..
					
					// 이제 전체 데이터 중 남은 헤더의 크기를 다시 계산해보자...
					// messageInfoLength
					// 가능한 헤더의 크기.
					int can_headerSize = MTU_MAX - pos;
					if ((messageInfoLength + 5) - (rememberPos * (breakCount + 1)) > 0) {
						//아직 남았다...
						b_info.block = (char*)malloc(MTU_MAX);
						memset(b_info.block, 0, MTU_MAX);
						memcpy(&b_info.block[0], tempBlock, pos);
						memcpy(&b_info.block[0], &data[rememberPos], can_headerSize);
						rememberPos += MTU_MAX;
					}
					else {
						// 남은 헤더의 크기가 가능한 헤더의 크기보다 작다!
						headerSizeOverlapMTU_MAX = false;
						b_info.block = (char*)malloc(pos + (messageInfoLength + 5) - (rememberPos * (breakCount + 1)));
						memcpy(&b_info.block[0], tempBlock, pos);
						memcpy(&b_info.block[pos], data, (messageInfoLength + 5) - (rememberPos * (breakCount + 1)));
						b_info.size = (messageInfoLength + 5) - (rememberPos * (breakCount + 1));
						rememberPos = (messageInfoLength + 5) - (rememberPos * (breakCount + 1));
					}
					free(tempBlock);
				}
			}
			else {
				// 헤더의 크기가 MTU 이하이다.
				// 혹은 헤더 블록 부분에 대한 처리가 완료되었다.
			}
		}
		breakCount++;
	}

	/////// 맨 마지막 단계에서 std::list<BLOCK_INFO> b_info_list;의 첫번째 값에
	// Block Count 부분을 수정해야한다.
	auto iter = b_info_list.begin();
	int blockCountPos = (1 + 4 + 4 + 1 + 2 + sendIdLength);
	byte BlockCount = (byte)breakCount;
	memcpy(&iter->block[blockCountPos], &BlockCount, sizeof(byte));

	free(sendID);

	return b_info_list;
}

WCHAR* CChattingRoom::SetSendMessage(CString message,int &len, SOCKET_INFO& socket_info)
{

	// type(1) + totalLength(4) + Message Info (4 + 1 + 2 + 가변) + Message (4 + 가변)

	int totalLength = 0;

	totalLength += 1;									// type
	totalLength += sizeof(int);							// totalLength
	totalLength += sizeof(int) + sizeof(BYTE) +
		sizeof(short) + _tcslen(socket_info.ID);		// Message Info
	totalLength += sizeof(int) + message.GetLength();	// Message
	
	WCHAR* sendMessage = (WCHAR*)malloc(totalLength*2);
	memset(sendMessage, 0, totalLength*2);
	WCHAR type = 'P';

	int pos = 0;

	memcpy(&sendMessage[pos], &type, sizeof(WCHAR));

	pos++;

	memcpy(&sendMessage[pos], &totalLength, sizeof(int));
	pos += sizeof(int);

	// setting Message Info...
	int messageInfoLength = 0;
	messageInfoLength += sizeof(int);
	WCHAR m_type = 'M';	// Message Info 에서의 Type 'M'은 메시지 타입이다. 추가적인 타입은 추후 별도로 정해야 한다.
	messageInfoLength += sizeof(WCHAR);
	short sendIDLength = _tcslen(socket_info.ID);
	messageInfoLength += sizeof(short);
	messageInfoLength += sendIDLength;

	memcpy(&sendMessage[pos], &messageInfoLength, sizeof(int));
	pos += sizeof(int);
	memcpy(&sendMessage[pos], &m_type, 1);
	pos += 1;
	memcpy(&sendMessage[pos], &sendIDLength, sizeof(short));
	pos += sizeof(short);
	memcpy(&sendMessage[pos], socket_info.ID, _tcslen(socket_info.ID) * 2);
	pos += sendIDLength;

	// setting Message
	int messageLength = message.GetLength();
	memcpy(&sendMessage[pos], &messageLength, sizeof(int));
	pos += sizeof(int);
	memcpy(&sendMessage[pos], message.GetBuffer(), messageLength * 2);
	pos += messageLength;

	len = pos;

	return sendMessage;
}

void CChattingRoom::OnBnClickedButtonSendMessage()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	WCHAR type = 'P';

	if (this->m_message.IsEmpty()) {
		return;
	}

	TRACE(_T("HERE1\n"));

	auto iter_c_info = theApp.socket.socket_map.find(theApp.socket.mapKey);

	int len = 0;

	TRACE(_T("HERE2\n"));

	WCHAR* sendMessage = SetSendMessage(this->m_message,len, iter_c_info->second);

	checksessionIdListThreadStart = false;

	send(iter_c_info->second.cSock, (char*)sendMessage,len*2, 0);
	
	free(sendMessage);

	TRACE(_T("HERE3\n"));

	m_editControl.SetWindowTextW(_T(""));

	checkchattingThreadStart = true;

	/*
	if (clnt_con->SendData(type, &iter_c_info->second.sendData[0], iter_c_info->second.sendDataLength, iter_c_info->second)) {
		// 추후에 이곳에 기존에 있던 채팅 내용들 같은 경우를 작성해야함.
		while (true) {
			break;
		}
	}
	*/
}


void CChattingRoom::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default

	int kkk = 0;

	CDialog::OnDropFiles(hDropInfo);
}


void CChattingRoom::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//CString s = _T("");
	//s.Format(_T("point.x = %d, point.y = %d | richEdit.x = %d, richEdit.y = %d\n"),
	//	point.x, point.y, richEditRect.left, richEditRect.bottom);
	//TRACE(s);

	//if ((point.x > this->richEditRect.left && point.x < this->richEditRect.right)) {
	//	TRACE(_T("point x in\n"));
	//}

	//if ((point.y < this->richEditRect.bottom && point.y > this->richEditRect.top)) {
	//	TRACE(_T("point y in\n"));
	//}

	//ScreenToClient(&point);
	//
	//CString s = _T("");
	//s.Format(_T("point.x = %d, point.y = %d\n"),
	//	point.x, point.y);
	//TRACE(s);
	//s.Format(_T("left = %d, right = %d, top = %d, bottom = %d\n"),
	//	richEditRect.left, richEditRect.right,richEditRect.top,richEditRect.bottom);
	//TRACE(s);

	//if ((point.x > this->richEditRect.left && point.x < this->richEditRect.right)
	//	&& (point.y < this->richEditRect.bottom && point.y > this->richEditRect.top)) {
	//	m_richEdit.SetWindowTextW(_T(""));
	//	TextOut_custom(_T("들어왔다."), RGB(255, 0, 0), m_richEdit);
	//}
	//else
	//	m_richEdit.SetWindowTextW(_T(""));
	//	TextOut_custom(_T("안 들어왔다."), RGB(0, 255, 0), m_richEdit);
	
	CDialog::OnMouseMove(nFlags, point);
}



void CChattingRoom::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//CString s = _T("");
	//s.Format(_T("point.x = %d, point.y = %d\n"),
	//	point.x, point.y);
	//TRACE(s);
	//CString s1 = _T("");
	//s1.Format(_T("left = %d, right = %d, top = %d, bottom = %d\n"),
	//	richEditRect.left, richEditRect.right, richEditRect.top, richEditRect.bottom);
	//AfxMessageBox(s + _T(" | ") + s1, MB_OK, 0);


	CDialog::OnLButtonDown(nFlags, point);
}


void CChattingRoom::OnEnChangeRichedit21()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CChattingRoom::OnDropfilesRichedit21(NMHDR* pNMHDR, LRESULT* pResult)
{
	int aaa = 0;

	ENDROPFILES* pEnDropFiles = reinterpret_cast<ENDROPFILES*>(pNMHDR);
	// TODO:  The control will not send this notification unless you override the
	// CDialog::OnInitDialog() function to send the EM_SETEVENTMASK message
	// to the control with the ENM_DROPFILES flag ORed into the lParam mask.

	// TODO:  Add your control notification handler code here
	int kkk = 0;


	*pResult = 0;
}

/////////
// CRichEditControl custom

//IMPLEMENT_DYNAMIC(CMyRichEditControl, CRichEditCtrl)
//
//CMyRichEditControl::CMyRichEditControl()
//{
//
//}
//
//CMyRichEditControl::~CMyRichEditControl()
//{
//
//}
//
//
//BEGIN_MESSAGE_MAP(CMyRichEditControl, CRichEditCtrl)
//	ON_NOTIFY_REFLECT(EN_QUERYACCEPTDATA, &CMyRichEditControl::OnQueryAcceptData)
//END_MESSAGE_MAP()
//
//LRESULT CMyRichEditControl::OnQueryAcceptData(WPARAM wParam, LPARAM lParam)
//{
//	// 여기에 EN_QUERYACCEPTDATA 메시지 처리 코드를 추가합니다.
//
//	// TRUE를 반환하면 데이터를 수락하고, FALSE를 반환하면 데이터를 거부합니다.
//	return TRUE;
//}

void CChattingRoom::OnDragdropdoneRichedit21(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

	int k = 0;

	*pResult = 0;
}

BOOL CChattingRoom::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_DROPFILES && pMsg->hwnd == m_editControl.GetSafeHwnd()) {
		HDROP hDrop = (HDROP)pMsg->wParam;

		UINT nFileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
		
		std::list<FILE_INFO> file_info_list;
		
		for (UINT i = 0; i < nFileCount; ++i) {
			// 파일 이름 얻기
			TCHAR szFilePath[MAX_PATH];
			DragQueryFile(hDrop, i, szFilePath, MAX_PATH);

			// 여기에서 파일 이름(szFilePath)을 사용하여 원하는 작업 수행
			// 예: 파일 이름을 출력
			CString strFileName = szFilePath;
			TRACE(_T("Dropped File: %s\n"), strFileName);

			FILE* pFile;
			errno_t err = _wfopen_s(&pFile, szFilePath, _T("rb"));
			if (err != 0) {
				wchar_t buf[1000];
				_wcserror_s(buf, err);
				AfxMessageBox(buf,MB_OK,0);
				return false;
			}

			fseek(pFile, 0, SEEK_END);
			long fSize = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);

			FILE_INFO f_info;
			f_info.file_name = strFileName;
			f_info.file_size = fSize;

			// 2023.11.28 만약 파일 크기가 홀수이면? +1을 해줘서 짝수로 만들어준다.
			// 물론 실제 파일 크기가 변경되는건 아니다.

			if (fSize % 2 != 0) {
				// 홀수
				f_info.file_data = (char*)malloc(fSize+1);
				memset(f_info.file_data, 0, fSize+1);
			}
			else {
				// 짝수
				f_info.file_data = (char*)malloc(fSize);
				memset(f_info.file_data, 0, fSize);
			}
			fread(f_info.file_data, 1, fSize, pFile);

			file_info_list.push_back(f_info);

			fclose(pFile);
		}
		auto iter_c_info = theApp.socket.socket_map.find(theApp.socket.mapKey);

		// nFileCount가 UINT -> byte로 줄어드는 이유는 최대 전송 파일의 갯수는 10개이기 때문이다.
		//char* sendMessage = Get_FileUpload_Completed(file_info_list, iter_c_info->second);
		//std::list<BLOCK_INFO> b_list = Get_FileUpload_Completed(file_info_list, iter_c_info->second);
		std::list<BLOCK_INFO> b_list = Get_FileUpload_Completed_Ver2(file_info_list, iter_c_info->second);
		
		for (auto iter = b_list.begin(); iter != b_list.end(); iter++) {
			clnt_con->JustSendData(iter->block, iter->size, iter_c_info->second.pTCP_SOCKET, iter_c_info->second.cSock);
			
		}
		
		DragFinish(hDrop);

		//::DragAcceptFiles(this->m_hWnd, true);
		//GetParent()->SendMessage(WM_DROPFILES, pMsg->wParam, pMsg->lParam);
		//::DragAcceptFiles(this->m_hWnd, false);
	}


	return CDialog::PreTranslateMessage(pMsg);
}
