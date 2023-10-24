#pragma once

#include "tcp/Client_TCP.h"

bool CRECV_CONTROL_CLIENT::Recv_Response(WCHAR* data, int& len, SOCKET_INFO& socket_info)
{
	WCHAR checkResponse = 0;
	memcpy(&checkResponse, &data[1], 1);

	if (checkResponse == 0x01)
		return true;
	else
		return false;
}

bool CRECV_CONTROL_CLIENT::Recv_ErrorMsg(WCHAR* data, int& len, SOCKET_INFO& socket_info)
{
	socket_info.sendDataLength = len;
	memcpy(socket_info.sendData, data, len);

	return true;
}

// 서버에서 전송받은 로그인 세션에 대한 정보들을 어떻게 클라이언트측에서 처리하여 list control에서 보이게 할것 인지?
bool CRECV_CONTROL_CLIENT::Recv_LoginSessionListInfo(WCHAR* data, int& len, SOCKET_INFO& socket_info)
{
	return true;
}
