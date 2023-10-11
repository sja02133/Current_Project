#pragma once

#include "../../GStreamer_MFC_Server/tcp/tcpSocket.h"

#include <atlstr.h>

class CRECV_CONTROL_CLIENT {
public:
	CRECV_CONTROL_CLIENT();
	// 2023.09.25 수정사항. 마지막에 true를 반환 할 때 총 문자열의 길이를 len에 할당해야한다.
	bool Recv_DeleteLoginSessionAll();
	bool Recv_LoginRequest(WCHAR* data, int& len, CLIENT_INFO& c_info);
	bool Recv_LoginResponse(WCHAR* data, int& len, CLIENT_INFO& c_info);
	bool Recv_IDExist(WCHAR* data, int& len, CLIENT_INFO& c_info);
	bool Recv_MembershipJoin(WCHAR* data, int& len, CLIENT_INFO& c_info);
	bool Recv_LoginOutRequest(WCHAR* data, int& len, CLIENT_INFO& c_info);
	bool Recv_AlreadyLoginSessionExist(WCHAR* data, int& len, CLIENT_INFO& c_info);
	bool Recv_LoginSessionList(WCHAR* data, int& len, CLIENT_INFO& c_info);
};

class CCLIENT_CONTROL
	: public CRECV_CONTROL_CLIENT {
public:
	typedef struct columndata {
		byte db_type;
		int len = 0;
		WCHAR* data = 0;
	}COLUMN_DATA;
public:

public:
	bool RecvData_Client(WCHAR* data, int& len, CLIENT_INFO& c_info);

	bool Send_Response(CLIENT_INFO* c_info, bool success);
	bool Send_Message(CLIENT_INFO* c_info);
};