#pragma once

/*

�̰��� Ŀ���� Ŭ���̾�Ʈ �ڵ尡 �����

Ŭ���̾�Ʈ ������ �߻��Ǵ� �۽�, ���ſ� ���� �ڵ�� ��� �̰��� �ۼ��� ��!

���� ����� tcp ���Ͽ���, Connect �� ������ ���� �����带 �ڵ����� ����� �ش�.

��� main ���������� �̰� CCLIENT_CONTROL�� Ŭ���� ������ ���� ���� �������� �Ѿ���� �Ѵ�.

*/

#include "../../Common/GStreamer_MFC/_TCP_/tcpSocket.h"

#include <atlstr.h>

class CRECV_CONTROL_CLIENT {
public:
	bool Recv_Response(WCHAR* data, int& len, SOCKET_INFO& socket_info);
	bool Recv_ErrorMsg(WCHAR* data, int& len, SOCKET_INFO& socket_info);
	bool Recv_LoginSessionListInfo(WCHAR* data, int& len, SOCKET_INFO& socket_info);
	
	//CRECV_CONTROL_CLIENT();
	// 2023.09.25 ��������. �������� true�� ��ȯ �� �� �� ���ڿ��� ���̸� len�� �Ҵ��ؾ��Ѵ�.
	//bool Recv_DeleteLoginSessionAll();
	//bool Recv_LoginRequest(WCHAR* data, int& len, SOCKET_INFO& c_info);
	//bool Recv_LoginResponse(WCHAR* data, int& len, SOCKET_INFO& c_info);
	//bool Recv_IDExist(WCHAR* data, int& len, SOCKET_INFO& c_info);
	//bool Recv_MembershipJoin(WCHAR* data, int& len, SOCKET_INFO& c_info);
	//bool Recv_LoginOutRequest(WCHAR* data, int& len, SOCKET_INFO& c_info);
	//bool Recv_AlreadyLoginSessionExist(WCHAR* data, int& len, SOCKET_INFO& c_info);
	//bool Recv_LoginSessionList(WCHAR* data, int& len, SOCKET_INFO& c_info);
};

class CSEND_CONTROL_CLIENT {
public:
	bool MakeSendData(WCHAR proto_type, WCHAR* data, int len, CTCP_SOCKET* pTCP_SOCKET, SOCKET cSock);
//	bool SetClientInfo_sendData(WCHAR* data, int len, CTCP_SOCKET* pTCP_SOCKET);
public:
	WCHAR* MakeSetTypeChar(WCHAR type, WCHAR* data, int& len);
//	WCHAR* MakeRequestLoginData(WCHAR type, WCHAR* data, int& len);
//	WCHAR* MakeRequestIDExist(WCHAR type, WCHAR* data, int& len);
//	WCHAR* MakeRequestMembershipJoin(WCHAR type, WCHAR* data, int& len);
//	WCHAR* MakeRequestLogOut(WCHAR type, WCHAR* data, int& len);
//	WCHAR* MakeRequestLoginSession(WCHAR type, WCHAR* data, int& len);
};
/*
class CCLIENT_CONTROL {
public:
	bool Recv_Response(WCHAR* data, int& len, SOCKET_INFO& c_info);
public:
	bool Send_Fail(SOCKET_INFO& c_info, int code);
	bool MakeErrorCode(int code, SOCKET_INFO& c_info);
	bool MakeErrorExistMsg(int code, SOCKET_INFO& c_info);
	bool Set_ErrorMsg(SOCKET_INFO& c_info, int code);
};
*/
class CCLIENT_CONTROL : public CTCP_SOCKET
	, public CRECV_CONTROL_CLIENT
	, public CSEND_CONTROL_CLIENT {
public:
	typedef struct columndata {
		byte db_type;
		int len = 0;
		WCHAR data[10000] = { 0, };
	}COLUMN_DATA;
public:
	CCLIENT_CONTROL();
	~CCLIENT_CONTROL();
public:
	SOCKET_INFO* m_socketInfo;
	bool RecvData_Client(WCHAR* data, int& len, SOCKET_INFO& socket_info);
	//bool Send_Response(SOCKET_INFO* c_info, bool success);
	//bool Send_Message(SOCKET_INFO* c_info);
	bool SendData(WCHAR type, WCHAR* data, int len,SOCKET_INFO& socket_info);
};