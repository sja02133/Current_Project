#pragma once

/*

이곳은 커스텀 서버 코드가 저장됨

서버측에서 발생되는 송신, 수신에 대한 코드는 모두 이곳에 작성할 것!

공통 모듈의 tcp 소켓에선, Accept 후 데이터 수신 쓰레드를 자동으로 만들어 준다.

고로 main 영역에서는 이곳 CSERVER_CONTROL의 클래스 생성을 통한 소켓 생성으로 넘어가져야 한다.

*/

#include "../Common/GStreamer_MFC/_TCP_/tcpSocket.h"

#include <atlstr.h>

class CRECV_CONTROL_SERVER {
public:
	CRECV_CONTROL_SERVER();
	// 2023.09.25 수정사항. 마지막에 true를 반환 할 때 총 문자열의 길이를 len에 할당해야한다.
	bool Recv_DeleteLoginSessionAll();
	bool Recv_LoginRequest(WCHAR* data, int& len, SOCKET_INFO& c_info);
	bool Recv_LoginResponse(WCHAR* data, int& len, SOCKET_INFO& c_info);
	bool Recv_IDExist(WCHAR* data, int& len, SOCKET_INFO& c_info);
	bool Recv_MembershipJoin(WCHAR* data, int& len, SOCKET_INFO& c_info);
	bool Recv_LoginOutRequest(WCHAR* data, int& len, SOCKET_INFO& c_info);
	bool Recv_AlreadyLoginSessionExist(WCHAR* data, int& len, SOCKET_INFO& c_info);
	bool Recv_LoginSessionList(WCHAR* data, int& len, SOCKET_INFO& c_info);
};

class CSERVER_CONTROL
	: public CTCP_SOCKET
	, public CRECV_CONTROL_SERVER {
public:
	typedef struct columndata {
		byte db_type;
		int len = 0;
		WCHAR* data = 0;
	}COLUMN_DATA;
public:

public:
	bool RecvData_Server(WCHAR* data, int& len, SOCKET_INFO& c_info);
	bool Send_Response(SOCKET_INFO* c_info, bool success);
	bool Send_Message(SOCKET_INFO* c_info);
};

