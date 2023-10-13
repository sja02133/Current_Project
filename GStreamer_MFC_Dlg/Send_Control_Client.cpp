#pragma once

#include "tcp/Client_TCP.h"

bool CSEND_CONTROL_CLIENT::MakeSendData(WCHAR proto_type, WCHAR* data, int len, CTCP_SOCKET* pTCP_SOCKET, SOCKET cSock)
{


	WCHAR* sendRealData = 0;
	switch (proto_type) {
	case _T('L'):
	case _T('I'):
	case _T('M'):
	case _T('O'):
	case _T('C'):
		sendRealData = MakeSetTypeChar(proto_type, data, len);
		break;
	//case _T('I'):
	//	sendRealData = MakeRequestIDExist(proto_type, data, len);
	//	break;
	//case _T('M'):
	//	sendRealData = MakeRequestMembershipJoin(proto_type, data, len);
	//	break;
	//case _T('O'):
	//	sendRealData = MakeRequestLogOut(proto_type, data, len);
	//	break;
	//case _T('C'):
	//	sendRealData = MakeRequestLoginSession(proto_type, data, len);
	//	break;
	default:
		sendRealData = nullptr;
		break;
	}

	if (sendRealData != nullptr)
	{
		//if (SetClientInfo_sendData(sendRealData, len, pTCP_SOCKET)) {
		//	std::map<CString, client_info>::iterator iter_c_info = pTCP_SOCKET->client_map.find(pTCP_SOCKET->mapKey);
		//	if (iter_c_info == pTCP_SOCKET->client_map.end())
		//		return false;

		//	auto& pCINFO = iter_c_info->second;
		//	pCINFO.last_type = proto_type;

		//	// 2023.09.25..
		//	// 만약 이곳에서 send를 그냥 해준다면???
		int k = send(cSock, (char*)sendRealData, len * 2, 0);
		if (k < 0)
			return false;
		//else
		//	return true;

		WCHAR* buffer[1000] = { 0, };

		k = recv(cSock, (char*)buffer, MAX_WCHAR_SIZE, 0);
		int kkk = 0;
	}
	return false;
}

WCHAR* CSEND_CONTROL_CLIENT::MakeSetTypeChar(WCHAR type, WCHAR* data, int& len)
{
	// 현재 이곳에서 메모리 침범 에러가 생기는것 같다. 

	WCHAR* totalStr = 0;

	if (len == 0) {
		// type만 오는 경우
		totalStr = (WCHAR*)malloc(2);
		memcpy(totalStr, &type, 2);
		return totalStr;
	}

	// 2023.09.05 현재 totalSize를 다시 계산하여 해결된 것으로 보이나 테스트 좀 더 필요..

	int typeSize = 1;

	int totalSize = (len * 2) + typeSize * 2 + (sizeof(int) * 2);

	totalStr = (WCHAR*)malloc(totalSize);	// 1 is type position, 4 is length position

	int pos = 0;

	memset(totalStr, 0, totalSize);
	memcpy(&totalStr[pos], &type, typeSize);
	pos += typeSize;

	len += 5;

	memcpy(&totalStr[pos], &len, sizeof(int) * 2);
	pos += 4;

	len -= 5;

	memcpy(&totalStr[pos], data, len * 2);

	len += 5;

	return totalStr;
}