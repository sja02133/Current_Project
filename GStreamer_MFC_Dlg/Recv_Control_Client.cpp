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

// �������� ���۹��� �α��� ���ǿ� ���� �������� ��� Ŭ���̾�Ʈ������ ó���Ͽ� list control���� ���̰� �Ұ� ����?
bool CRECV_CONTROL_CLIENT::Recv_LoginSessionListInfo(WCHAR* data, int& len, SOCKET_INFO& socket_info)
{
	return true;
}
