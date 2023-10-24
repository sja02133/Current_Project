#pragma once

/*

�̰��� Ŀ���� ���� �ڵ尡 �����

���������� �߻��Ǵ� �۽�, ���ſ� ���� �ڵ�� ��� �̰��� �ۼ��� ��!

���� ����� tcp ���Ͽ���, Accept �� ������ ���� �����带 �ڵ����� ����� �ش�.

��� main ���������� �̰� CSERVER_CONTROL�� Ŭ���� ������ ���� ���� �������� �Ѿ���� �Ѵ�.

*/

#include "../Common/GStreamer_MFC/_TCP_/tcpSocket.h"

#include <atlstr.h>

class CRECV_CONTROL_SERVER {
public:
	//CRECV_CONTROL_SERVER();
	// 2023.09.25 ��������. �������� true�� ��ȯ �� �� �� ���ڿ��� ���̸� len�� �Ҵ��ؾ��Ѵ�.
	bool Recv_DeleteLoginSessionAll();
	bool Recv_LoginRequest(WCHAR* data, int& len, SOCKET_INFO& c_info);
	bool Recv_LoginResponse(WCHAR* data, int& len, SOCKET_INFO& c_info);
	bool Recv_IDExist(WCHAR* data, int& len, SOCKET_INFO& c_info);
	bool Recv_MembershipJoin(WCHAR* data, int& len, SOCKET_INFO& c_info);
	bool Recv_LoginOutRequest(WCHAR* data, int& len, SOCKET_INFO& c_info);
	bool Recv_AlreadyLoginSessionExist(WCHAR* data, int& len, SOCKET_INFO& c_info);
	bool Recv_LoginSessionList(WCHAR* data, int& len, SOCKET_INFO& c_info);
};


class CSEND_CONTROL_SERVER {
public:
	bool MakeSendData(WCHAR proto_type, WCHAR* data, int len, CTCP_SOCKET* pTCP_SOCKET, SOCKET cSock);
	bool SetClientInfo_sendData(WCHAR* data, int len, CTCP_SOCKET* pTCP_SOCKET);
	bool Set_ErrorMsg(SOCKET_INFO& c_info, int code);
	bool Send_Fail(SOCKET_INFO& c_info, int code);
	bool MakeErrorExistMsg(int code, SOCKET_INFO& c_info);
	bool MakeErrorCode(int code, SOCKET_INFO& c_info);
public:
	WCHAR* MakeSetTypeChar(WCHAR type, WCHAR* data, int& len);
	WCHAR* MakeRequestLoginData(WCHAR type, WCHAR* data, int& len);
	WCHAR* MakeRequestIDExist(WCHAR type, WCHAR* data, int& len);
	WCHAR* MakeRequestMembershipJoin(WCHAR type, WCHAR* data, int& len);
	WCHAR* MakeRequestLogOut(WCHAR type, WCHAR* data, int& len);
	WCHAR* MakeRequestLoginSession(WCHAR type, WCHAR* data, int& len);
};

class CSERVER_CONTROL : public CTCP_SOCKET
	, public CRECV_CONTROL_SERVER
	, public CSEND_CONTROL_SERVER {
public:
	typedef struct columndata {
		byte db_type;
		int len = 0;
		WCHAR* data = 0;
	}COLUMN_DATA;
public:
	CSERVER_CONTROL();
	~CSERVER_CONTROL();
public:
	bool RecvData_Server(WCHAR* data, int& len, SOCKET_INFO& c_info);
	bool Send_Response(SOCKET_INFO* c_info, bool success);
	bool Send_Message(SOCKET_INFO* c_info);
};

