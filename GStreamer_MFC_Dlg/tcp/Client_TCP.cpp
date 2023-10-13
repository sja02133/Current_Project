#include "Client_TCP.h"

CCLIENT_CONTROL::CCLIENT_CONTROL()
{
	//
}
CCLIENT_CONTROL::~CCLIENT_CONTROL()
{
	//
}

// �׳� �����͸� ������. type�� ���� ó��.
bool CCLIENT_CONTROL::SendData(WCHAR type, WCHAR* data, int len, SOCKET_INFO& socket_info)
{
	// initialized SOCKET_INFO option..
	this->SetInitialize_CLIENT_INFO(&socket_info);

	if (MakeSendData(type, data, len, socket_info.pTCP_SOCKET, socket_info.cSock)) {
		return true;
	}
	else
		return false;
}

// Ư�� �����͸� ���� ������ type�� ���� ó����Ų��.
bool CCLIENT_CONTROL::RecvData_Client(WCHAR* data, int& len, SOCKET_INFO& socket_info)
{
	WCHAR firstByte = data[0];
	switch (firstByte) {
	case 'R':
		// Response ������ ��.
		socket_info.checkResponse = true;
		if (Recv_Response(data, len, socket_info)) {	
			return true;
		}
		else {
			return false;
		}
		break;
	case 'S':
		// ���� ���� ���� ���� ������ ���ŵ�
		break;
	}


	return true;
}

