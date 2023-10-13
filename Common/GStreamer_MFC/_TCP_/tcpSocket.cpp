#include "tcpSocket.h"
#include <sstream>

#define MAXBUF 1024

//CTCP_SOCKET tempClass;

#include "../../../GStreamer_MFC_Server/Server_TCP.h"
#include "../../../GStreamer_MFC_Dlg/tcp/Client_TCP.h"

void WINAPI Thread_Recv(void* arg);

// do not modify this function!
template<typename ... Args>
CString string_format(const CString& format, Args ... args)
{
	size_t size = snprintf(nullptr, 0, format.GetString(), args ...) + 1; // Extra space for '\0'
	if (size <= 0) { throw std::runtime_error("Error during formatting."); }
	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.GetString(), args ...);
	return CString(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}
//

CTCP_SOCKET::~CTCP_SOCKET()
{
	// 종료 혹은 파괴 시

	// 1. std::map<CString, client_info> client_map; 의 client_map에 대한 정보들을 삭제한다.
	auto iter = this->socket_map.begin();
	int check = 0;
	for (; iter != this->socket_map.end(); iter++) {
		// malloc된 데이터 지우기.
		if (iter->second.sendDataLength > 0)
			this->SetInitialize_CLIENT_INFO(iter);

		// 쓰레드 핸들 종료 시키기
		if (iter->second.checkSend == true)
			SuspendThread(iter->second.clientToSendThread);
		else if (iter->second.checkRecv == true)
			SuspendThread(iter->second.clientToRecvThread);

		check++;
	}

	if (check > 0)
		this->socket_map.clear();
	//
}

void WINAPI Thread_Accept(void* arg)
{

	fd_set reads, temps;
	SOCKET_INFO* socket_info = (SOCKET_INFO*)arg;

	//c_info->pTCP_SOCKET->listener;

	struct timeval timeout;

	while (true) {
		//Sleep(1000);

		FD_ZERO(&reads);
		FD_SET(socket_info->pTCP_SOCKET->listener, &reads);

		temps = reads;

		timeout.tv_sec = 0;
		timeout.tv_usec = 100000;

		int s = select(socket_info->pTCP_SOCKET->listener + 1, &temps, 0, 0, &timeout);

		if (s < 0)
			break;
		else if (s == 0)
			continue;

		for (int i = 0; i < reads.fd_count; i++) {
			if (reads.fd_array[i] == socket_info->pTCP_SOCKET->listener) {
				// 연결 요청
				SOCKET hCltSock = accept(socket_info->pTCP_SOCKET->listener, nullptr, nullptr);
				if (hCltSock == INVALID_SOCKET) {
					closesocket(socket_info->pTCP_SOCKET->listener);
					WSACleanup();
					return;
				}
				else {
					// accept 성공!
					socket_info->cSock = hCltSock;
					HANDLE hHandle = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(__stdcall*)(void*))Thread_Recv, (void*)socket_info, 0, 0);
					socket_info->pTCP_SOCKET->client_HANDLE_map.insert(std::make_pair(socket_info->ipPort, hHandle));
				}
			}
		}
	}
}

void WINAPI Thread_Recv(void* arg)
{
	fd_set reads, temps;
	SOCKET_INFO* socket_info = (SOCKET_INFO*)arg;

	//c_info->pTCP_SOCKET->listener;

	struct timeval timeout;

	while (true) {
		//Sleep(1000);

		FD_ZERO(&reads);
		FD_SET(socket_info->cSock, &reads);

		temps = reads;

		timeout.tv_sec = 0;
		timeout.tv_usec = 100000;

		int s = select(socket_info->cSock + 1, &temps, 0, 0, &timeout);

		if (s < 0)
			break;
		else if (s == 0)
			continue;

		if (FD_ISSET(socket_info->cSock, &reads)) {
			// read
			WCHAR buf[MAX_WCHAR_SIZE] = { 0, };
			int len = recv(socket_info->cSock, (char*)buf, MAX_WCHAR_SIZE, 0);
			if (len > 0) {
				// 이곳에 데이터 수신 부 처리하는 커스텀 처리를 할것!
				//socket_info->pTCP_SOCKET->RecvData_Server(buf, len, *socket_info);
				/*
				if (socket_info->checkServer == 1) {
					// 서버
#ifndef SOCKET_CLIENT
					CSERVER_CONTROL serv_con;
					serv_con.RecvData_Server(buf, len, *socket_info);
#endif
				}
				else {
					// 클라이언트
					CCLIENT_CONTROL clnt_con;
					clnt_con.RecvData_Client(buf, len, *socket_info);
				}
				*/

#ifdef SOCKET_CLIENT
				CCLIENT_CONTROL clnt_con;
				clnt_con.RecvData_Client(buf, len, *socket_info);
#else
				CSERVER_CONTROL serv_con;
				serv_con.RecvData_Server(buf, len, *socket_info);
#endif
			}
			else if (len < 0) {
				// lose connection
				return;
			}
			else {
				// other problem OR 0
				int aaa = 0;
			}
		}
	}
}

bool test = false;

//void WINAPI send_data(void* arg)
//{
//	CLIENT_INFO* c_info = (CLIENT_INFO*)arg;
//
//	WCHAR ipPort[30] = { 0, };
//	
//	memcpy(&ipPort[0], c_info->ipPort, 30*2);
//
//	CString key = ipPort;
//
//	int n = 0;
//	WCHAR buf[MAXBUF] = { 0 };
//
//	auto iter_c_info = c_info->pTCP_SOCKET->GetIteratorTarget(c_info->pTCP_SOCKET, key);
//	if (iter_c_info == c_info->pTCP_SOCKET->client_map.end())
//		return;
//
//	delete arg;
//
//	while (true) {
//		Sleep(DELAY_TIME);
//		if (iter_c_info->second.status != 0) {
//			// Exist send data..
//			if (iter_c_info->second.sendData[0] == 'C')
//				int kkk = 0;
//
//			n = send(iter_c_info->second.cSock, (char*)iter_c_info->second.sendData, iter_c_info->second.sendDataLength*2, 0);
//
//			if(iter_c_info->second.sendDataLength > 0)
//				wmemset(iter_c_info->second.sendData,0,MAX_WCHAR_SIZE);
//			wmemset(buf, 0, MAXBUF);
//			n = recv(iter_c_info->second.cSock, (char*)buf, MAXBUF, 0);
//			if (n > 0) {
//				// 수신된 데이터가 있다...
//				c_info->pTCP_SOCKET->RecvData(buf, n, iter_c_info->second);
//				iter_c_info->second.status = 0;
//				wmemset(iter_c_info->second.sendData, 0, MAX_WCHAR_SIZE);
//				wmemcpy(iter_c_info->second.sendData, buf, n);
//				iter_c_info->second.sendDataLength = (n / 2); // -> n(수신된 byte)에서 나누기 2를 하는 이유는 send 보낼때 곱하기 2를 해서 보내기 때문에.CString이라..
//				iter_c_info->second.last_type = buf[0];
//				if (iter_c_info->second.last_type == 'R') {
//					WCHAR checkResponseResult = 0;
//					wmemcpy(&checkResponseResult, &buf[1], 1);
//					if (checkResponseResult == 0x01)
//						iter_c_info->second.last_type_success = 1;
//					else
//						iter_c_info->second.last_type_success = 0;
//				}
//				else if (iter_c_info->second.last_type == 'E') {
//					iter_c_info->second.last_type_success = 0;
//				}
//			}
//			iter_c_info->second.status = 0;
//		}
//		else if (n == -1)
//			break;
//	}
//}

bool CTCP_SOCKET::Initialize(bool checkRecv)
{
	if (!WSAStartUp_CHECK()) {
		return false;
	}
	if (!CreateSocket_CHECK()) {
		return false;
	}
	if (!Bind_CHECK(checkRecv)) {
		return false;
	}
	if (checkRecv) {
		if (!Listen_CHECK()) {
			return false;
		}
	}

	if (checkRecv) {
		// Server	
		while (true) {
			SOCKET client_sock = accept(this->listener, nullptr, nullptr);
			if (client_sock == INVALID_SOCKET) {
				closesocket(listener);
				WSACleanup();
				return false;
			}
			//printf("Accept!\n");
			this->clientInfo = new SOCKET_INFO;
			memcpy(&this->clientInfo->cSock, &client_sock, sizeof(SOCKET));
			int addrLen = sizeof(this->clientInfo->clientAddr);
			getpeername(this->clientInfo->cSock, (sockaddr*)&this->clientInfo->clientAddr, &addrLen);
			char* clientIP = inet_ntoa(this->clientInfo->clientAddr.sin_addr);
			WCHAR WCHAR_clientIP[30] = { 0, };
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, clientIP, strlen(clientIP), WCHAR_clientIP, 256);
			int clientPort = ntohs(this->clientInfo->clientAddr.sin_port);
			CString portNum = _T("");
			portNum.Format(_T("%d"), clientPort);
			CString key = CString(WCHAR_clientIP) + _T(":") + portNum;
			memcpy(&this->clientInfo->ipPort[0], key.GetBuffer(), key.GetLength()*2);
			this->clientInfo->checkRecv = true;
			//this->client_map.insert(std::make_pair(key, *c_info));	
			this->clientInfo->pTCP_SOCKET = this;
			this->clientInfo->checkServer = 1;
			HANDLE hHandle = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(__stdcall*)(void*))Thread_Recv, (void*)this->clientInfo, 0, 0);
			this->server_HANDLE = hHandle;
		}
	}
	else {
		// Client
		// 클라이언트인 경우엔 서버랑 1:1 통신만 하므로, 따로 계속하여 while문으로 잡아두는게 아닌 아예 함수 종료를 실시한다.
		int count = 0;
		int check_connect = 0;
		while (check_connect = connect(this->listener, (SOCKADDR*)&address, sizeof(address)) == SOCKET_ERROR) {
			Sleep(SOCKET_CONNECT_TIMEOUT);
			check_connect = connect(this->listener, (SOCKADDR*)&address, sizeof(address));
			count++;
			if (count == 10) {
				DWORD a = GetLastError();
				printf("connect() error!");
				break;
			}
		}
		if (check_connect == SOCKET_ERROR)
			return false;

		Sleep(DELAY_TIME);

		this->clientInfo = new SOCKET_INFO;
		memcpy(&this->clientInfo->cSock, &this->listener, sizeof(SOCKET));
		int addrLen = sizeof(this->clientInfo->clientAddr);
		getpeername(this->clientInfo->cSock, (sockaddr*)&this->clientInfo->clientAddr, &addrLen);
		char* clientIP = inet_ntoa(this->clientInfo->clientAddr.sin_addr);
		WCHAR WCHAR_clientIP[30] = { 0, };
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, clientIP, strlen(clientIP), WCHAR_clientIP, 256);
		int clientPort = ntohs(this->clientInfo->clientAddr.sin_port);
		CString portNum = _T("");
		portNum.Format(_T("%d"), clientPort);
		CString key = CString(WCHAR_clientIP) + _T(":") + portNum;
		memcpy(&this->clientInfo->ipPort[0], key.GetBuffer(), key.GetLength() * 2);
		this->clientInfo->checkSend= true;
		this->mapKey = key;
		//this->client_map.insert(std::make_pair(key, *c_info));	
		this->clientInfo->pTCP_SOCKET = this;
		this->clientInfo->checkServer = 0;
		//HANDLE hHandle = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(__stdcall*)(void*))send_data, (void*)c_info, 0, 0);
		HANDLE hHandle = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(__stdcall*)(void*))Thread_Recv, (void*)this->clientInfo, 0, 0);

		this->client_HANDLE = hHandle;

		//if(hHandle != nullptr)
		//	this->client_map.at(key).clientToSendThread = hHandle;
	}
	return true;
}


bool CTCP_SOCKET::WSAStartUp_CHECK()
{
	if (WSAStartup(MAKEWORD(2, 2), &this->wsa) != 0)
		return false;
	else
		return true;
}

bool CTCP_SOCKET::CreateSocket_CHECK()
{
	//this->listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	this->listener = socket(PF_INET, SOCK_STREAM, 0);
	if (this->listener == INVALID_SOCKET)
		return false;
	else
		return true;
}

bool CTCP_SOCKET::Bind_CHECK(bool checkRecv)
{
	if (checkRecv) {
		// Server
		memset(&address, 0, sizeof(sockaddr_in));
		address.sin_family = AF_INET;
		address.sin_port = htons(SERVER_PORT);
		address.sin_addr.s_addr = htonl(INADDR_ANY);
		int check = bind(this->listener, (sockaddr*)&address, sizeof(sockaddr_in));

		if (check == 0)
			return true;
		else {
			// 에러 메시지..
			CString error = GetBindErrorMsg(check);
			return false;
		}
	}
	else {
		// Client
		memset(&address, 0, sizeof(sockaddr_in));
		address.sin_family = AF_INET;
		address.sin_port = htons(SERVER_PORT);
		address.sin_addr.s_addr = inet_addr(SERVER_IP);
		return true;
	}
}
bool CTCP_SOCKET::Bind_CHECK()
{
	sockaddr_in address;
	memset(&address, 0, sizeof(sockaddr_in));
	address.sin_family = AF_INET;
	address.sin_port = htons(SERVER_PORT);
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	int check = bind(this->listener, (sockaddr*)&address, sizeof(sockaddr_in));

	if (check == 0)
		return true;
	else {
		// 에러 메시지..
		CString error = GetBindErrorMsg(check);
		return false;
	}
}

CString CTCP_SOCKET::GetBindErrorMsg(int errorCode)
{
	CString str = "";

	switch (errorCode) {
	case WSANOTINITIALISED:
		str = "WSAStartup 이 성공적으로 호출된 적이 없다.";
		break;
	case WSAENETDOWN:
		str = "네트워크 서브시스템에서 실패";
		break;
	case WSAEACCES:
		str = "요청한 주소가 브로드캐스트 주소인데 setsockopt()로 SO_BROADCAST 가 설정되지 않았다.";
		break;
	case WSAEADDRINUSE:
		str = "동일한 주소/포트를 다른 프로세스에서 사용하고 있다. SO_REUSEADDR(:12)을 이용해서 주소를 재사용할 것을 명시함으로 해결할 수 있다.";
		break;
	case WSAEADDRNOTAVAIL:
		str = "지정된 주소가 올바른 주소가 아님";
		break;
	case WSAEFAULT:
		str = "name 혹은 namelen 인자가 잘못되었을 경우. namelen이 너무 작거나, name이 잘못된 주소형태를 포함하고 있을 경우";
		break;
	case WSAEINVAL:
		str = "이미 bind된 소켓임";
		break;
	case WSAENOBUFS:
		str = "너무 많은 연결로 남아있는 버퍼가 없음";
		break;
	case WSAENOTSOCK:
		str = "socket() 함수로 생성된 소켓이 아님";
		break;
	default:
		str = "찾을 수 없는 에러";
		break;
	}
	return str;
}

bool CTCP_SOCKET::Listen_CHECK()
{
	// 성공 시 0, 실패 시 -1
	int check = listen(listener, SOMAXCONN);
	if (check == 0)
		return true;
	else
		return false;
}

std::map<CString, SOCKET_INFO>::iterator CTCP_SOCKET::GetIteratorTarget(CTCP_SOCKET* pTCP_SOCKET,CString mapKey)
{
	std::map<CString, SOCKET_INFO>::iterator iter = pTCP_SOCKET->socket_map.find(mapKey);
	return iter;
}

void CTCP_SOCKET::SetInitialize_CLIENT_INFO(std::map<CString, SOCKET_INFO>::iterator iter)
{
	iter->second.last_type = 0;
	iter->second.last_type_success = -1;
	if (iter->second.sendDataLength > 0) {
		wmemset(iter->second.sendData, 0, 1024);
		iter->second.sendDataLength = 0;
	}
}

// SOCKET_INFO 구조체의 정보를 초기화 시킨다.
void CTCP_SOCKET::SetInitialize_CLIENT_INFO(SOCKET_INFO* socket_info)
{
	socket_info->last_type = 0;
	socket_info->last_type_success = -1;
	if (socket_info->sendDataLength > 0) {
		wmemset(socket_info->sendData, 0, 1024);
		socket_info->sendDataLength = 0;
	}
}

void CTCP_SOCKET::GetServerErrorMsg(CString& str, SOCKET_INFO& c_info)
{
	int pos = 0;

	int totalLength = c_info.sendDataLength;

	char* tempChar = (char*)malloc(totalLength);
	memcpy(tempChar, c_info.sendData, totalLength);

	pos++;	//pass to type 1 byte

	int length = get_byteToInt(&tempChar[pos]);
	pos += sizeof(int);

	int errorCode = get_byteToInt(&tempChar[pos]);
	pos += sizeof(int);

	WCHAR* errorMsg = (WCHAR*)malloc(totalLength - pos);
	memcpy(errorMsg, &c_info.sendData[pos], (totalLength - pos)*2);

	str = errorMsg;

	free(errorMsg);
}


bool CTCP_SOCKET::SendData(WCHAR type, WCHAR* data, int len)
{
	// 이곳에서 보낼 데이터를 만든다..

	this->SetInitialize_CLIENT_INFO(this->clientInfo);

	if (this->mapKey.IsEmpty())
		return false;

	//if (MakeSendData(type, data, len, this, this->clientInfo->cSock)) {
	//	return true;
	//}
	//else
	//	return false;

	//

	return true;
}