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