#include "Client_TCP.h"

CCLIENT_CONTROL::CCLIENT_CONTROL()
{
	//
}
CCLIENT_CONTROL::~CCLIENT_CONTROL()
{
	//
}

// 그냥 데이터를 보낸다. type에 따라 처리.
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

// 특정 데이터를 수신 받으면 type에 따라 처리시킨다.
bool CCLIENT_CONTROL::RecvData_Client(WCHAR* data, int& len, SOCKET_INFO& socket_info)
{
	WCHAR firstByte = data[0];
	switch (firstByte) {
	case 'R':
		// Response 응답이 옴.
		socket_info.checkResponse = true;
		if (Recv_Response(data, len, socket_info)) {	
			return true;
		}
		else {
			return false;
		}
		break;
	case 'S':
		// 현재 접속 중인 유저 정보가 수신됨
		break;
	}


	return true;
}

