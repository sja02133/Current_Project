#include "tcpSocket.h"
#include <sstream>
#include "../byteControl.h"

#define MAXBUF 1024

//CTCP_SOCKET tempClass;

void WINAPI Thread_Recv(void* arg);

CTCP_SOCKET::~CTCP_SOCKET()
{
	// ���� Ȥ�� �ı� ��

	// 1. std::map<CString, client_info> client_map; �� client_map�� ���� �������� �����Ѵ�.
	auto iter = this->client_map.begin();
	int check = 0;
	for (; iter != this->client_map.end(); iter++) {
		// malloc�� ������ �����.
		if (iter->second.sendDataLength > 0)
			this->SetInitialize_CLIENT_INFO(iter);

		// ������ �ڵ� ���� ��Ű��
		if (iter->second.checkSend == true)
			SuspendThread(iter->second.clientToSendThread);
		else if (iter->second.checkRecv == true)
			SuspendThread(iter->second.clientToRecvThread);

		check++;
	}

	if (check > 0)
		this->client_map.clear();
	//
}

template<typename ... Args>
CString string_format(const CString& format, Args ... args)
{
	size_t size = snprintf(nullptr, 0, format.GetString(), args ...) + 1; // Extra space for '\0'
	if (size <= 0) { throw std::runtime_error("Error during formatting."); }
	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.GetString(), args ...);
	return CString(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

void WINAPI recv_data(void *arg)
{
	printf("���� ����\n");
	CLIENT_INFO* c_info = (CLIENT_INFO*)arg;
	WCHAR ipPort[20] = { 0, };
	memcpy(&ipPort[0], c_info->ipPort, 20*2);
	CString key = ipPort;

	//auto iter = c_info->pTCP_SOCKET->client_map.find(key);
	auto iter = c_info->pTCP_SOCKET->GetIteratorTarget(c_info->pTCP_SOCKET, key);

	if (iter == c_info->pTCP_SOCKET->client_map.end())
		return;

	delete arg;

	int n = 0;
	WCHAR buf[MAXBUF] = { 0 };
	while (true) {
		Sleep(DELAY_TIME);
		wmemset(buf, 0, MAXBUF);
		n = recv(iter->second.cSock, (char*)buf, MAXBUF, 0);
		if (n > 0) {
			if (c_info->pTCP_SOCKET->RecvData(buf, n, iter->second)) {
				if (iter->second.checkResponse == false) {
					c_info->pTCP_SOCKET->Send_Message(&iter->second);
					iter->second.checkResponse = true;
				}
				else
					c_info->pTCP_SOCKET->Send_Response(&iter->second, true);
			}
			else {
				if (iter->second.errorCode == 0)
					c_info->pTCP_SOCKET->Send_Response(&iter->second, false);
				else
					c_info->pTCP_SOCKET->Send_Fail(iter->second,iter->second.errorCode);
			}
		}
		else if (n == -1)
			break;
	}
}

void WINAPI Thread_Accept(void* arg)
{
	fd_set reads, temps;
	CLIENT_INFO* c_info = (CLIENT_INFO*)arg;

	//c_info->pTCP_SOCKET->listener;

	struct timeval timeout;

	while (true) {
		//Sleep(1000);

		FD_ZERO(&reads);
		FD_SET(c_info->pTCP_SOCKET->listener, &reads);

		temps = reads;

		timeout.tv_sec = 0;
		timeout.tv_usec = 100000;

		int s = select(c_info->pTCP_SOCKET->listener + 1, &temps, 0, 0, &timeout);

		if (s < 0)
			break;
		else if (s == 0)
			continue;

		for (int i = 0; i < reads.fd_count; i++) {
			if (reads.fd_array[i] == c_info->pTCP_SOCKET->listener) {
				// ���� ��û
				SOCKET hCltSock = accept(c_info->pTCP_SOCKET->listener, nullptr, nullptr);
				if (hCltSock == INVALID_SOCKET) {
					closesocket(c_info->pTCP_SOCKET->listener);
					WSACleanup();
					return;
				}
				else {
					// accept ����!
					c_info->cSock = hCltSock;
					HANDLE hHandle = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(__stdcall*)(void*))Thread_Recv, (void*)c_info, 0, 0);
					c_info->pTCP_SOCKET->client_HANDLE_map.insert(std::make_pair(c_info->ipPort, hHandle));
				}
			}
		}
	}
}

void WINAPI Thread_Recv(void* arg)
{
	fd_set reads, temps;
	CLIENT_INFO* c_info = (CLIENT_INFO*)arg;

	//c_info->pTCP_SOCKET->listener;

	struct timeval timeout;

	while (true) {
		//Sleep(1000);

		FD_ZERO(&reads);
		FD_SET(c_info->cSock, &reads);

		temps = reads;

		timeout.tv_sec = 0;
		timeout.tv_usec = 100000;

		int s = select(c_info->cSock + 1, &temps, 0, 0, &timeout);

		if (s < 0)
			break;
		else if (s == 0)
			continue;

		if (FD_ISSET(c_info->cSock, &reads)) {
			// read
			WCHAR buf[MAX_WCHAR_SIZE] = { 0, };
			int len = recv(c_info->cSock, (char*)buf, MAX_WCHAR_SIZE, 0);
			if (len > 0) {
				c_info->pTCP_SOCKET->RecvData(buf, len, *c_info);
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

void WINAPI send_data(void* arg)
{
	CLIENT_INFO* c_info = (CLIENT_INFO*)arg;

	WCHAR ipPort[30] = { 0, };
	
	memcpy(&ipPort[0], c_info->ipPort, 30*2);

	CString key = ipPort;

	int n = 0;
	WCHAR buf[MAXBUF] = { 0 };

	auto iter_c_info = c_info->pTCP_SOCKET->GetIteratorTarget(c_info->pTCP_SOCKET, key);
	if (iter_c_info == c_info->pTCP_SOCKET->client_map.end())
		return;

	delete arg;

	while (true) {
		Sleep(DELAY_TIME);
		if (iter_c_info->second.status != 0) {
			// Exist send data..
			if (iter_c_info->second.sendData[0] == 'C')
				int kkk = 0;

			n = send(iter_c_info->second.cSock, (char*)iter_c_info->second.sendData, iter_c_info->second.sendDataLength*2, 0);

			if(iter_c_info->second.sendDataLength > 0)
				wmemset(iter_c_info->second.sendData,0,MAX_WCHAR_SIZE);
			wmemset(buf, 0, MAXBUF);
			n = recv(iter_c_info->second.cSock, (char*)buf, MAXBUF, 0);
			if (n > 0) {
				// ���ŵ� �����Ͱ� �ִ�...
				c_info->pTCP_SOCKET->RecvData(buf, n, iter_c_info->second);
				iter_c_info->second.status = 0;
				wmemset(iter_c_info->second.sendData, 0, MAX_WCHAR_SIZE);
				wmemcpy(iter_c_info->second.sendData, buf, n);
				iter_c_info->second.sendDataLength = (n / 2); // -> n(���ŵ� byte)���� ������ 2�� �ϴ� ������ send ������ ���ϱ� 2�� �ؼ� ������ ������.CString�̶�..
				iter_c_info->second.last_type = buf[0];
				if (iter_c_info->second.last_type == 'R') {
					WCHAR checkResponseResult = 0;
					wmemcpy(&checkResponseResult, &buf[1], 1);
					if (checkResponseResult == 0x01)
						iter_c_info->second.last_type_success = 1;
					else
						iter_c_info->second.last_type_success = 0;
				}
				else if (iter_c_info->second.last_type == 'E') {
					iter_c_info->second.last_type_success = 0;
				}
			}
			iter_c_info->second.status = 0;
		}
		else if (n == -1)
			break;
	}
}

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
		WCHAR firsWCHAR = '1';
		CLIENT_INFO temp;
		//this->RecvData(&firsWCHAR, 1,temp);
		
		while (true) {
			SOCKET client_sock = accept(this->listener, nullptr, nullptr);
			if (client_sock == INVALID_SOCKET) {
				closesocket(listener);
				WSACleanup();
				return false;
			}
			printf("Accept!\n");

			//client_info* c_info = new client_info;
			//memcpy(&c_info->cSock, &client_sock, sizeof(SOCKET));
			//int addrLen = sizeof(c_info->clientAddr);
			//getpeername(c_info->cSock, (sockaddr*)&c_info->clientAddr, &addrLen);
			//char* clientIP = inet_ntoa(c_info->clientAddr.sin_addr);
			//WCHAR WCHAR_clientIP[30] = { 0, };
			//MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, clientIP, strlen(clientIP), WCHAR_clientIP, 256);
			//int clientPort = ntohs(c_info->clientAddr.sin_port);
			//CString portNum = _T("");
			//portNum.Format(_T("%d"), clientPort);
			//CString key = CString(WCHAR_clientIP) + _T(":") + portNum;
			//memcpy(&c_info->ipPort[0], key.GetBuffer(), key.GetLength()*2);
			//c_info->checkRecv = true;
			//this->client_map.insert(std::make_pair(key, *c_info));		
			//c_info->pTCP_SOCKET = this;
			
			this->clientInfo = new CLIENT_INFO;
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
			
			HANDLE hHandle = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(__stdcall*)(void*))Thread_Recv, (void*)this->clientInfo, 0, 0);
			this->server_HANDLE = hHandle;
		}
	}
	else {
		// Client
		// Ŭ���̾�Ʈ�� ��쿣 ������ 1:1 ��Ÿ� �ϹǷ�, ���� ����Ͽ� while������ ��Ƶδ°� �ƴ� �ƿ� �Լ� ���Ḧ �ǽ��Ѵ�.
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

		//client_info* c_info = new client_info;
		//c_info->cSock = this->listener;
		//int addrLen = sizeof(c_info->clientAddr);
		//getpeername(c_info->cSock, (sockaddr*)&c_info->clientAddr, &addrLen);
		//char* clientIP = inet_ntoa(c_info->clientAddr.sin_addr);
		//WCHAR WCHAR_clientIP[30] = { 0, };
		//MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, clientIP, strlen(clientIP), WCHAR_clientIP, 256);
		//int clientPort = ntohs(c_info->clientAddr.sin_port);
		//CString portNum = _T("");
		//portNum.Format(_T("%d"), clientPort);
		//CString key = CString(WCHAR_clientIP) + _T(":") + portNum;
		//memcpy(&c_info->ipPort[0], key.GetBuffer(), key.GetLength()*2);
		//c_info->checkSend = true;
		//this->mapKey = key;
		//c_info->pTCP_SOCKET = this;
		//this->client_map.insert(std::make_pair(key, *c_info));

		//c_info->pTCP_SOCKET = 0;
		//delete c_info;

		this->clientInfo = new CLIENT_INFO;
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
			// ���� �޽���..
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
		// ���� �޽���..
		CString error = GetBindErrorMsg(check);
		return false;
	}
}

CString CTCP_SOCKET::GetBindErrorMsg(int errorCode)
{
	CString str = "";

	switch (errorCode) {
	case WSANOTINITIALISED:
		str = "WSAStartup �� ���������� ȣ��� ���� ����.";
		break;
	case WSAENETDOWN:
		str = "��Ʈ��ũ ����ý��ۿ��� ����";
		break;
	case WSAEACCES:
		str = "��û�� �ּҰ� ��ε�ĳ��Ʈ �ּ��ε� setsockopt()�� SO_BROADCAST �� �������� �ʾҴ�.";
		break;
	case WSAEADDRINUSE:
		str = "������ �ּ�/��Ʈ�� �ٸ� ���μ������� ����ϰ� �ִ�. SO_REUSEADDR(:12)�� �̿��ؼ� �ּҸ� ������ ���� ��������� �ذ��� �� �ִ�.";
		break;
	case WSAEADDRNOTAVAIL:
		str = "������ �ּҰ� �ùٸ� �ּҰ� �ƴ�";
		break;
	case WSAEFAULT:
		str = "name Ȥ�� namelen ���ڰ� �߸��Ǿ��� ���. namelen�� �ʹ� �۰ų�, name�� �߸��� �ּ����¸� �����ϰ� ���� ���";
		break;
	case WSAEINVAL:
		str = "�̹� bind�� ������";
		break;
	case WSAENOBUFS:
		str = "�ʹ� ���� ����� �����ִ� ���۰� ����";
		break;
	case WSAENOTSOCK:
		str = "socket() �Լ��� ������ ������ �ƴ�";
		break;
	default:
		str = "ã�� �� ���� ����";
		break;
	}
	return str;
}

bool CTCP_SOCKET::Listen_CHECK()
{
	// ���� �� 0, ���� �� -1
	int check = listen(listener, SOMAXCONN);
	if (check == 0)
		return true;
	else
		return false;
}

std::map<CString, client_info>::iterator CTCP_SOCKET::GetIteratorTarget(CTCP_SOCKET* pTCP_SOCKET,CString mapKey)
{
	std::map<CString, client_info>::iterator iter = pTCP_SOCKET->client_map.find(mapKey);
	return iter;
}

void CTCP_SOCKET::SetInitialize_CLIENT_INFO(std::map<CString, client_info>::iterator iter)
{
	iter->second.last_type = 0;
	iter->second.last_type_success = -1;
	if (iter->second.sendDataLength > 0) {
		wmemset(iter->second.sendData, 0, 1024);
		iter->second.sendDataLength = 0;
	}
}

void CTCP_SOCKET::SetInitialize_CLIENT_INFO(CLIENT_INFO* c_info)
{
	c_info->last_type = 0;
	c_info->last_type_success = -1;
	if (c_info->sendDataLength > 0) {
		wmemset(c_info->sendData, 0, 1024);
		c_info->sendDataLength = 0;
	}
}

void CTCP_SOCKET::GetServerErrorMsg(CString& str, CLIENT_INFO& c_info)
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