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



