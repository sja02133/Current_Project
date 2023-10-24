
#include "../../Common/GStreamer_MFC/_TCP_/tcpSocket.h"

#include "../../Common/GStreamer_MFC/byteControl/byteControl.h"

#include "../../GStreamer_MFC_Server/Server_TCP.h"

CSERVER_CONTROL::CSERVER_CONTROL()
{
	//
}

CSERVER_CONTROL::~CSERVER_CONTROL()
{
	// �Ҹ��� ȣ�� ������ CSERVER_CONTROL -> CTCP_CONTROL
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
		// ���� ���� ����
		Recv_DeleteLoginSessionAll();
		c_info.pTCP_SOCKET->checkFirstStart = false;
	}

	switch (firstByte) {
	case 'L':
	{
		// login ��û
		if (Recv_LoginRequest(data, len, c_info))
			checkResult = true;
		else
			checkResult = false;
		break;
	}
	case 'I':
	{
		// ID �ߺ� �˻�
		if (Recv_IDExist(data, len, c_info))
			checkResult = true;
		else
			checkResult = false;
		break;
	}
	case 'M':
		// ȸ������ ��û
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
		// �α׾ƿ� ���� ����
		if (Recv_LoginOutRequest(data, len, c_info)) {
			printf("%s �α׾ƿ� ����\n", c_info.ID);
			//wmemset(c_info.ID, 0, _tcsclen(c_info.ID));
			memset(c_info.ID, 0, 100);
			return true;	// �α׾ƿ� �� Ŭ���̾�Ʈ���� ���ٸ� �޽����� ������ �ʿ�� ����.
			//checkResult = true;
		}
		else {
			return false;
			//checkResult = false;
		}
		break;
	case 'A':
		// �α��� ���� �ߺ� �˻�
		if (Recv_AlreadyLoginSessionExist(data, len, c_info)) {
			return true;
		}
		else {
			//Send_Fail(c_info, 100);	//�ڵ�� ���� �ӽ�...
			return false;
		}
		break;
	case 'C':
		// ���� �α��� ���� ���� ����Ʈ �۽�
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
		// �ڵ� ���� �ʿ�..
		Send_Response(&c_info, false);
	}

	////////////////////////////////////
	// ���� �α��� ���� Ȥ�� �α׾ƿ� ��Ȳ �߻� ��, �ٸ� Ŭ���̾�Ʈ�鿡�� Ư�� �۾��� �����ش�.
	// -> �� �۾��� Response ������ Ŭ���̾�Ʈ������ ������ �� �۾��Ѵ�.(������ ���� �� �ֱ� ����...)
	// 1. �α��� �õ� -> ���� ��!
	// 2023.10.23 ��������
	// ���������� Ŭ���̾�Ʈ���� �α��� ���� Response�� ������ �̰����� �ٷ� �α����� Ŭ���̾�Ʈ����
	// ���� ���� ���� ������ �ѱ�� ������ �����. ���, Ŭ���̾�Ʈ������ ���������� �� �α��� ���� Response�� ���� ������ ������� �Ѵ�?



	////////////////////////////////////

	return true;
}


