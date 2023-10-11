#pragma once

#include <iostream>
#include <stdint.h>

using namespace std;

class LOGIN_INFO {
public:
	time_t loginRequestTime;
	time_t loginResponseTime;
	string id;
	string password;
};

class LOGIN_REQUEST : public LOGIN_INFO{
public:
	// 받은 패킷에서 로그인 정보를 가져온다.
	bool getLoginInfo(unsigned char* data);

	// 클라이언트 측으로부터 온 로그인 정보를 검증한다.
	uint32_t checkLoginInfo();

	// 클라이언트 측으로 로그인 성공 유무 상태를 응답한다.
	bool reponseLoginStatus(bool status);

};

