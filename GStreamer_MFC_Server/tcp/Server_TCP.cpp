#include "tcpSocket.h"

#include "../byteControl.h"

bool CSERVER_CONTROL::Send_Message(CLIENT_INFO* c_info)
{
	int check = send(c_info->cSock, (char*)c_info->sendData, c_info->sendDataLength * 2, 0);
	if (check > 0)
		return true;
	else
		return false;
}

bool CSERVER_CONTROL::Send_Response(CLIENT_INFO* c_info, bool success)
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

bool CSERVER_CONTROL::RecvData_Server(WCHAR* data, int& len, CLIENT_INFO& c_info)
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
		if (Recv_MembershipJoin(data, len, c_info)) {
			//memset(c_info.ID, 0, strlen(c_info.ID));
			wmemset(c_info.ID, 0, _tcsclen(c_info.ID));
			return true;
		}
		else
			return false;
		break;
	case 'O':
		// 로그아웃 세션 제거
		if (Recv_LoginOutRequest(data, len, c_info)) {
			printf("%s 로그아웃 성공\n", c_info.ID);
			wmemset(c_info.ID, 0, _tcsclen(c_info.ID));
			return true;
		}
		else
			return false;
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
			return true;
		}
		else
			return false;
		break;
	case 'S':
		// 현재 로그인 중인 세션 리스트 수신
		return true;
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
			Send_Message(&c_info);
	}
	else if (c_info.checkResponse == false && checkResult == false) {
		// message(error code)
		Send_Message(&c_info);
	}
	else {
		// 코드 수정 필요..
		Send_Response(&c_info, false);
	}

	return true;
}


