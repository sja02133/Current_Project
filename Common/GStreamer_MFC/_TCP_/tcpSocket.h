#pragma once

/*

TCP ���� ��� �κи� ������ ��!

���� : (FUNC) Initialize() �Լ����� Client �� Server ����..
		Client : Connect �� ���� Thread_Recv �Լ��� ������ ���� �ش� ��� ����
		Server : Accept Waiting ���� Accept �� Thread_Recv �Լ��� ������ ���� Accept Waiting..
other �ڵ忡�� ���� : Thread_Recv �Լ� ���ο��� �ʿ��� �Լ� ������ ��..
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
#include "../byteControl/byteControl.h"
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

typedef struct socket_info {
	// ������ Ŭ���̾�Ʈ�� ��������.
	bool checkServer = 0;	// 0 : Ŭ���̾�Ʈ, 1 : ����

	// ����
	SOCKET cSock;
	WCHAR ipPort[30] = { 0, };
	int errorCode = 0;
	WCHAR sendData[MAX_WCHAR_SIZE] = { 0, };
	unsigned int sendDataLength;

	CTCP_SOCKET* pTCP_SOCKET;

	time_t loginTime;	// ���� ���� ���� ���� ���

	// Ŭ���̾�Ʈ
	HANDLE clientToRecvThread;

	// ����
	HANDLE clientToSendThread;
	WCHAR ID[100] = { 0, };


	bool checkResponse = true;
	sockaddr_in clientAddr;
	int status = 0;
	WCHAR last_type = 0;
	int last_type_success = -1;	// -1 : �������� �� �� ����, 0 : ����, 1 : ����
	bool checkRecv = false;
	bool checkSend = false;
	//bool checkExit = false;
	
}SOCKET_INFO;


class CTCP_SOCKET
{
public:
	CTCP_SOCKET()
	{

	}
public:
	CTCP_SOCKET(bool checkRecv)
	{
		Initialize(checkRecv);
	}
public:
	virtual ~CTCP_SOCKET();
	bool Initialize(bool checkRecv);	// Recv => 1, Send => 0
	bool WSAStartUp_CHECK();
	bool CreateSocket_CHECK();
	bool Bind_CHECK();
	bool Bind_CHECK(bool checkRecv);	// Recv => 1, Send => 0 | Client �϶�(��, Send�϶�)�� bind�� �ʿ� ������ Recv �϶� bind�� �ʿ������� �׳� �ִ´�.
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

	// ���� Ŭ���̾�Ʈ�� ��, key(SERVER IP) ����
	CString mapKey = _T("");

	// 2023.09.25 �߰�
	HANDLE server_HANDLE = NULL;
	HANDLE client_HANDLE = NULL;
	std::map<CString,HANDLE> client_HANDLE_map;
	bool checkFirstStart = true;
	SOCKET_INFO* clientInfo;

	// �ڵ� ����...
	bool SendData(WCHAR type, WCHAR* data, int len);
};