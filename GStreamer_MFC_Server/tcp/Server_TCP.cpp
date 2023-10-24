
#include "../../Common/GStreamer_MFC/_TCP_/tcpSocket.h"

#include "../../Common/GStreamer_MFC/byteControl/byteControl.h"

#include "../../GStreamer_MFC_Server/Server_TCP.h"

CSERVER_CONTROL::CSERVER_CONTROL()
{
	//
}

CSERVER_CONTROL::~CSERVER_CONTROL()
{
	// 소멸자 호출 순서는 CSERVER_CONTROL -> CTCP_CONTROL
}

bool CSERVER_CONTROL::Send_Message(SOCKET_INFO* c_info)
{
	int check = send(c_info->cSock, (char*)c_info->sendData, c_info->sendDataLength * 2, 0);
	if (check > 0)
		return true;
	else
		return false;
}

bool CSERVER_CONTROL::Send_Response(SOCKET_INFO* c_info, bool success)
{
	WCHAR type = 'R';

	WCHAR* sendData = (WCHAR*)malloc(4);
	int pos = 0;
	memcpy(&sendData[pos], &type, 2);
	pos++;
	memcpy(&sendData[pos], &success, sizeof(bool)*2);
	pos++;
	int check = 0;

	check = send(c_info->cSock, (char*)sendData, pos*2, 0);

	free(sendData);
	if (check > 0)
		return true;
	else
		return false;
}

bool CSERVER_CONTROL::RecvData_Server(WCHAR* data, int& len, SOCKET_INFO& c_info)
{
	WCHAR firstByte = data[0];

	bool checkResult = false;

	if (c_info.pTCP_SOCKET->checkFirstStart == true && c_info.pTCP_SOCKET->client_HANDLE == NULL) {
		// 서버 최초 실행
		Recv_DeleteLoginSessionAll();
		c_info.pTCP_SOCKET->checkFirstStart = false;
	}

	switch (firstByte) {
	case 'L':
	{
		// login 요청
		if (Recv_LoginRequest(data, len, c_info))
			checkResult = true;
		else
			checkResult = false;
		break;
	}
	case 'I':
	{
		// ID 중복 검사
		if (Recv_IDExist(data, len, c_info))
			checkResult = true;
		else
			checkResult = false;
		break;
	}
	case 'M':
		// 회원가입 요청
		c_info.checkResponse = true;
		if (Recv_MembershipJoin(data, len, c_info)) {
			//memset(c_info.ID, 0, strlen(c_info.ID));
			wmemset(c_info.ID, 0, _tcsclen(c_info.ID));
			checkResult = true;
		}
		else
			checkResult = false;
		break;
	case 'O':
		// 로그아웃 세션 제거
		if (Recv_LoginOutRequest(data, len, c_info)) {
			printf("%s 로그아웃 성공\n", c_info.ID);
			//wmemset(c_info.ID, 0, _tcsclen(c_info.ID));
			memset(c_info.ID, 0, 100);
			return true;	// 로그아웃 시 클라이언트에게 별다른 메시지를 보내줄 필요는 없다.
			//checkResult = true;
		}
		else {
			return false;
			//checkResult = false;
		}
		break;
	case 'A':
		// 로그인 세션 중복 검사
		if (Recv_AlreadyLoginSessionExist(data, len, c_info)) {
			return true;
		}
		else {
			//Send_Fail(c_info, 100);	//코드는 현재 임시...
			return false;
		}
		break;
	case 'C':
		// 현재 로그인 중인 세션 리스트 송신
		if (Recv_LoginSessionList(data, len, c_info)) {
			checkResult = true;
		}
		else
			checkResult = false;
		break;
	default:
		return true;
		break;
	}

	if (c_info.checkResponse == true && checkResult == true) {
		// response(true)
		Send_Response(&c_info, true);
	}
	else if (c_info.checkResponse == true && checkResult == false) {
		// response(false)
		if (c_info.errorCode == 0)
			Send_Response(&c_info, false);
		else
			Send_Fail(c_info, c_info.errorCode);
			//Send_Message(&c_info);
	}
	else if (c_info.checkResponse == false && checkResult == false) {
		// message(error code)
		Send_Message(&c_info);
	}
	else if (c_info.checkResponse == false && checkResult == true) {
		Send_Message(&c_info);
	}
	else {
		// 코드 수정 필요..
		Send_Response(&c_info, false);
	}

	////////////////////////////////////
	// 만약 로그인 성공 혹은 로그아웃 상황 발생 시, 다른 클라이언트들에게 특정 작업을 보내준다.
	// -> 이 작업은 Response 정보를 클라이언트측으로 전송한 뒤 작업한다.(순서상 꼬일 수 있기 때문...)
	// 1. 로그인 시도 -> 성공 시!
	// 2023.10.23 수정사항
	// 서버측에서 클라이언트한테 로그인 성공 Response를 보낸뒤 이곳에서 바로 로그인한 클라이언트한테
	// 현재 유저 정보 정보를 넘기면 문제가 생긴다. 고로, 클라이언트측에서 서버측에서 온 로그인 성공 Response에 대한 응답을 보내줘야 한다?



	////////////////////////////////////

	return true;
}


