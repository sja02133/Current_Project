#pragma once

/*

TCP 공통 모듈 부분만 선언할 것!

범위 : (FUNC) Initialize() 함수에서 Client 와 Server 구분..
		Client : Connect 된 이후 Thread_Recv 함수를 구동한 다음 해당 모듈 종료
		Server : Accept Waiting 이후 Accept 시 Thread_Recv 함수를 구동한 다음 Accept Waiting..
other 코드에서 사용법 : Thread_Recv 함수 내부에서 필요한 함수 구현할 것..
*/




#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <string>
#include <map>
#include <list>
#include <winsock2.h>
#include <thread>

#include <process.h>

#include "../OLEDB/POLEDB.h"

#include "../ERROR_CODE/errorCode.h"

#include <atlstr.h>

#include "../../../GStreamer_MFC_Server/Server_TCP.h"

#pragma comment(lib,"ws2_32.lib")

class CTCP_SOCKET;

constexpr auto SERVER_PORT = 7000;
constexpr auto SERVER_IP = "192.168.2.239";
//constexpr auto SERVER_IP = "127.0.0.1";

#define SOCKET_CONNECT_TIMEOUT 300	// Client일 경우 Connect를 SOCKET_CONNECT_TIMEOUT 간격으로 10번 실행하고 안되면 connect 실패 처리한다.
#define SOCKET_RESPONSE_TIMEOUT 100	// Client일 경우, 특정 버퍼를 서버측으로 전송한 뒤 응답까지 SOCKET_RESPONSE_TIMEOUT 간격으로 10번 실행하고 안되면 실패처리한다.
#define DELAY_TIME	10	// 안정화를 위한 딜레이 시간.

#define MAX_WCHAR_SIZE 9000

//typedef struct client_info {
//	bool checkResponse = true;
//	sockaddr_in clientAddr;
//	int status = 0;
//	WCHAR last_type = 0;
//	int last_type_success = -1;	// -1 : 성공유무 알 수 없음, 0 : 실패, 1 : 성공
//	SOCKET cSock;
//	WCHAR ipPort[30] = { 0, };
//	bool checkRecv = false;
//	HANDLE clientToRecvThread;
//	bool checkSend = false;
//	HANDLE clientToSendThread;
//	int errorCode = 0;
//	WCHAR sendData[MAX_WCHAR_SIZE] = { 0, };
//	unsigned int sendDataLength;
//	time_t loginTime;
//	CTCP_SOCKET* pTCP_SOCKET;
//	bool checkExit = false;
//	WCHAR ID[100] = { 0, };
//}CLIENT_INFO;

typedef struct socket_info {
	// 서버와 클라이언트를 구분하자.
	bool checkServer = 0;	// 0 : 클라이언트, 1 : 서버

	// 공통
	SOCKET cSock;
	WCHAR ipPort[30] = { 0, };
	int errorCode = 0;
	WCHAR sendData[MAX_WCHAR_SIZE] = { 0, };
	unsigned int sendDataLength;

	CTCP_SOCKET* pTCP_SOCKET;

	time_t loginTime;	// 추후 최초 접속 시점 기록

	// 클라이언트
	HANDLE clientToRecvThread;

	// 서버
	HANDLE clientToSendThread;
	WCHAR ID[100] = { 0, };


	bool checkResponse = true;
	sockaddr_in clientAddr;
	int status = 0;
	WCHAR last_type = 0;
	int last_type_success = -1;	// -1 : 성공유무 알 수 없음, 0 : 실패, 1 : 성공
	bool checkRecv = false;
	bool checkSend = false;
	//bool checkExit = false;
	
}SOCKET_INFO;

class CMAP_CONTROL_CUSTOM {
public:
	CString MakeKey(CString ip, CString port);
};

//class CRECV_CONTROL {
//public:
//	CRECV_CONTROL();
//	// 2023.09.25 수정사항. 마지막에 true를 반환 할 때 총 문자열의 길이를 len에 할당해야한다.
//
//	// 서버 자체
//	bool Recv_DeleteLoginSessionAll();
//
//	// C -> S, 서버 수신
//	bool Recv_LoginRequest(WCHAR* data, int& len, CLIENT_INFO& c_info);
//
//	// C -> S, 서버 수신
//	bool Recv_IDExist(WCHAR* data, int& len, CLIENT_INFO& c_info);
//
//	// C -> S, 서버 수신
//	bool Recv_MembershipJoin(WCHAR* data, int& len, CLIENT_INFO& c_info);
//
//	// C -> S, 서버 수신
//	bool Recv_LoginOutRequest(WCHAR* data, int& len, CLIENT_INFO& c_info);
//
//	// C -> S, 서버 수신
//	bool Recv_AlreadyLoginSessionExist(WCHAR* data, int& len, CLIENT_INFO& c_info);
//
//	// C -> S, 서버 수신
//	bool Recv_LoginSessionList(WCHAR* data, int& len, CLIENT_INFO& c_info);
//};

class CCLIENT_CONTROL {
public:
	bool Recv_Response(WCHAR* data, int& len, SOCKET_INFO& c_info);
public:
	bool Send_Fail(SOCKET_INFO& c_info, int code);
	bool MakeErrorCode(int code, SOCKET_INFO& c_info);
	bool MakeErrorExistMsg(int code, SOCKET_INFO& c_info);
	bool Set_ErrorMsg(SOCKET_INFO& c_info, int code);
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



class CTCP_SOCKET : public CMAP_CONTROL_CUSTOM
	//public CSERVER_CONTROL
{
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
	bool Bind_CHECK(bool checkRecv);	// Recv => 1, Send => 0 | Client 일땐(즉, Send일때)는 bind는 필요 없으나 Recv 일땐 bind가 필요함으로 그냥 넣는다.
	bool Listen_CHECK();
	CString GetBindErrorMsg(int errorCode);
	std::map<CString, SOCKET_INFO>::iterator GetIteratorTarget(CTCP_SOCKET* pTCP_SOCKET,CString mapKey);
	void SetInitialize_CLIENT_INFO(std::map<CString, SOCKET_INFO>::iterator iter);
	void SetInitialize_CLIENT_INFO(SOCKET_INFO* socket_info);
	void GetServerErrorMsg(CString& str, SOCKET_INFO& c_info);
public:
	WSADATA wsa;
	SOCKET listener;
	sockaddr_in address;

	std::map<CString, SOCKET_INFO> socket_map;
	std::list<SOCKET_INFO> socket_list;

	// 만약 클라이언트일 때, key(SERVER IP) 저장
	CString mapKey = _T("");

	// 2023.09.25 추가
	HANDLE server_HANDLE = NULL;
	HANDLE client_HANDLE = NULL;
	std::map<CString,HANDLE> client_HANDLE_map;
	bool checkFirstStart = true;
	SOCKET_INFO* clientInfo;

	// 코드 수정...
	bool RecvData_Server(WCHAR* data, int& len, SOCKET_INFO& c_info);
	bool SendData(WCHAR type, WCHAR* data, int len);
};