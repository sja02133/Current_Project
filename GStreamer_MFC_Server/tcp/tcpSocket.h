#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <string>
#include <map>
#include <winsock2.h>
#include <thread>

#include <process.h>

#include "../POLEDB.h"

#include "../errorCode.h"

#include <atlstr.h>

#pragma comment(lib,"ws2_32.lib")

class CTCP_SOCKET;

constexpr auto SERVER_PORT = 7000;
constexpr auto SERVER_IP = "192.168.2.239";
//constexpr auto SERVER_IP = "127.0.0.1";

#define SOCKET_CONNECT_TIMEOUT 300	// Client�� ��� Connect�� SOCKET_CONNECT_TIMEOUT �������� 10�� �����ϰ� �ȵǸ� connect ���� ó���Ѵ�.
#define SOCKET_RESPONSE_TIMEOUT 100	// Client�� ���, Ư�� ���۸� ���������� ������ �� ������� SOCKET_RESPONSE_TIMEOUT �������� 10�� �����ϰ� �ȵǸ� ����ó���Ѵ�.
#define DELAY_TIME	10	// ����ȭ�� ���� ������ �ð�.

#define MAX_WCHAR_SIZE 9000

typedef struct client_info {
	bool checkResponse = true;
	sockaddr_in clientAddr;
	int status = 0;
	WCHAR last_type = 0;
	int last_type_success = -1;	// -1 : �������� �� �� ����, 0 : ����, 1 : ����
	SOCKET cSock;
	WCHAR ipPort[30] = { 0, };
	bool checkRecv = false;
	HANDLE clientToRecvThread;
	bool checkSend = false;
	HANDLE clientToSendThread;
	int errorCode = 0;
	WCHAR sendData[MAX_WCHAR_SIZE] = { 0, };
	unsigned int sendDataLength;
	time_t loginTime;
	CTCP_SOCKET* pTCP_SOCKET;
	bool checkExit = false;
	WCHAR ID[100] = { 0, };
}CLIENT_INFO;

class CMAP_CONTROL_CUSTOM {
public:
	CString MakeKey(CString ip, CString port);
};

class CSERVER_CONTROL {
public:
	typedef struct columndata {
		byte db_type;
		int len = 0;
		WCHAR* data = 0;
	}COLUMN_DATA;
public:

public:
	bool Send_Response(CLIENT_INFO* c_info, bool success);
	bool Send_Message(CLIENT_INFO* c_info);
};

class CCLIENT_CONTROL {
public:
	bool Recv_Response(WCHAR* data, int& len, CLIENT_INFO& c_info);
public:
	bool Send_Fail(CLIENT_INFO& c_info, int code);
	bool MakeErrorCode(int code, CLIENT_INFO& c_info);
	bool MakeErrorExistMsg(int code, CLIENT_INFO& c_info);
	bool Set_ErrorMsg(CLIENT_INFO& c_info, int code);
};

// FUNC NUM ����Ʈ (�߰� �� �� �� ���� �߰�)
enum RECV_FUNC_NO {
	FC_RV_LOGIN_REQUEST,
	FC_RV_ID_EXIST,

	MAX_FC_RV_NO	// <- �̰� �Ʒ��� FUNC NUM ���� ����
};

class CRECV_CONTROL {
public:
	CRECV_CONTROL();
	// 2023.09.25 ��������. �������� true�� ��ȯ �� �� �� ���ڿ��� ���̸� len�� �Ҵ��ؾ��Ѵ�.
	bool Recv_DeleteLoginSessionAll();
	bool Recv_LoginRequest(WCHAR* data, int& len, CLIENT_INFO& c_info);
	bool Recv_LoginResponse(WCHAR* data, int& len, CLIENT_INFO& c_info);
	bool Recv_IDExist(WCHAR* data, int& len, CLIENT_INFO& c_info);
	bool Recv_MembershipJoin(WCHAR* data, int& len, CLIENT_INFO& c_info);
	bool Recv_LoginOutRequest(WCHAR* data, int& len, CLIENT_INFO& c_info);
	bool Recv_AlreadyLoginSessionExist(WCHAR* data, int& len, CLIENT_INFO& c_info);
	bool Recv_LoginSessionList(WCHAR* data, int& len, CLIENT_INFO& c_info);

public:
	typedef bool(CRECV_CONTROL::* BOOL)(WCHAR*, int&, CLIENT_INFO&);
	BOOL FC_RV_FUNC_LIST[MAX_FC_RV_NO];
};

class CSEND_CONTROL {
public:
	bool MakeSendData(WCHAR proto_type, WCHAR* data, int len, CTCP_SOCKET* pTCP_SOCKET,SOCKET cSock);
	bool SetClientInfo_sendData(WCHAR* data, int len, CTCP_SOCKET* pTCP_SOCKET);
public:
	WCHAR* MakeSetTypeChar(WCHAR type, WCHAR* data, int& len);
	WCHAR* MakeRequestLoginData(WCHAR type, WCHAR* data, int& len);
	WCHAR* MakeRequestIDExist(WCHAR type, WCHAR* data, int& len);
	WCHAR* MakeRequestMembershipJoin(WCHAR type, WCHAR* data, int& len);
	WCHAR* MakeRequestLogOut(WCHAR type, WCHAR* data, int& len);
	WCHAR* MakeRequestLoginSession(WCHAR type, WCHAR* data, int& len);
};



class CTCP_SOCKET : public CMAP_CONTROL_CUSTOM,
	public CSEND_CONTROL,
	public CRECV_CONTROL,
	public CSERVER_CONTROL{
public:
	CTCP_SOCKET()
	{

	}
	CTCP_SOCKET(bool checkRecv)
	{
		Initialize(checkRecv);
	}
	~CTCP_SOCKET();
	bool Initialize(bool checkRecv);	// Recv => 1, Send => 0
	bool WSAStartUp_CHECK();
	bool CreateSocket_CHECK();
	bool Bind_CHECK();
	bool Bind_CHECK(bool checkRecv);	// Recv => 1, Send => 0 | Client �϶�(��, Send�϶�)�� bind�� �ʿ� ������ Recv �϶� bind�� �ʿ������� �׳� �ִ´�.
	bool Listen_CHECK();
	CString GetBindErrorMsg(int errorCode);
	std::map<CString, client_info>::iterator GetIteratorTarget(CTCP_SOCKET* pTCP_SOCKET,CString mapKey);
	void SetInitialize_CLIENT_INFO(std::map<CString, client_info>::iterator iter);
	void SetInitialize_CLIENT_INFO(CLIENT_INFO* c_info);
	void GetServerErrorMsg(CString& str,CLIENT_INFO& c_info);
public:
	WSADATA wsa;
	SOCKET listener;
	sockaddr_in address;
	std::map<CString, client_info> client_map;

	// ���� Ŭ���̾�Ʈ�� ��, key ����
	CString mapKey = _T("");

	// 2023.09.25 �߰�
	HANDLE server_HANDLE = NULL;
	HANDLE client_HANDLE = NULL;
	std::map<CString,HANDLE> client_HANDLE_map;
	bool checkFirstStart = true;
	CLIENT_INFO* clientInfo;

	// �ڵ� ����...
	bool RecvData(WCHAR* data, int& len, CLIENT_INFO& c_info);
	bool SendData(WCHAR type, WCHAR* data, int len);
};