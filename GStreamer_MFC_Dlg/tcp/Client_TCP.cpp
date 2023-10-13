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

	switch (type) {
	case 'R':
		break;
	}


	return true;
}

// 특정 데이터를 수신 받으면 type에 따라 처리시킨다.
bool CCLIENT_CONTROL::RecvData_Client(WCHAR* data, int& len, SOCKET_INFO& socket_info)
{
	WCHAR firstByte = data[0];
	switch (firstByte) {
	case 'R':
		// Response 응답이 옴.
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

