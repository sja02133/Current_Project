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

	socket_info.last_type = type;

	if (MakeSendData(type, data, len, socket_info.pTCP_SOCKET, socket_info.cSock)) {
		return true;
	}
	else
		return false;
}

// Ư�� �����͸� ���� ������ type�� ���� ó����Ų��.
bool CCLIENT_CONTROL::RecvData_Client(WCHAR* data, int& len, SOCKET_INFO& socket_info)
{
	auto iter = socket_info.pTCP_SOCKET->socket_map.find(socket_info.pTCP_SOCKET->mapKey);

	WCHAR firstByte = data[0];
	bool checkSuccess = false;
	switch (firstByte) {
	case 'R':
		// Response ������ ��.
		iter->second.checkResponse = true;

		if (Recv_Response(data, len, iter->second)) {
			checkSuccess = true;
		}
		else {
			checkSuccess = false;
		}
		break;
	case 'E':
		if (Recv_ErrorMsg(data, len, iter->second)) {
			checkSuccess = false;
		}
		else {
			iter->second.sendDataLength = 0;
			checkSuccess = false;
		}
		break;
	case 'S':
		// ���� ���� ���� ���� ������ ���ŵ�
		memcpy(iter->second.sendData, data, len);
		iter->second.sendDataLength = len;
		break;
	}

	iter->second.last_type = firstByte;

	iter->second.last_type_success = checkSuccess;

	return checkSuccess;
}

