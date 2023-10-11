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
	// ���� ��Ŷ���� �α��� ������ �����´�.
	bool getLoginInfo(unsigned char* data);

	// Ŭ���̾�Ʈ �����κ��� �� �α��� ������ �����Ѵ�.
	uint32_t checkLoginInfo();

	// Ŭ���̾�Ʈ ������ �α��� ���� ���� ���¸� �����Ѵ�.
	bool reponseLoginStatus(bool status);

};

