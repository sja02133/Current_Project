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

	switch (type) {
	case 'R':
		break;
	}


	return true;
}

// Ư�� �����͸� ���� ������ type�� ���� ó����Ų��.
bool CCLIENT_CONTROL::RecvData_Client(WCHAR* data, int& len, SOCKET_INFO& socket_info)
{
	WCHAR firstByte = data[0];
	switch (firstByte) {
	case 'R':
		// Response ������ ��.
		if (Recv_Response(data, len, socket_info)) {
			socket_info.checkResponse = true;
			return true;
		}
		else {
			socket_info.checkResponse = true;
			return false;
		}
		break;
	}


	return true;
}

