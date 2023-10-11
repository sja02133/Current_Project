
#include "login.h"

// 받은 패킷에서 로그인 정보를 가져온다.
bool LOGIN_REQUEST::getLoginInfo(unsigned char* data)
{

	return true;
}

// 클라이언트 측으로부터 온 로그인 정보를 검증한다.
uint32_t LOGIN_REQUEST::checkLoginInfo()
{
	uint32_t returnType = 0;

	// 아이디 검증 ( DB측에서 중복검사. )
	
	// 비밀번호 검증 ( 조건 : 특수문자(!,@,#) <- 3개 중에 하나 포함 + 알파벳 + 숫자, 띄어쓰기 없어야함)
	if (this->password.find('!', 0)) {

	}



	return returnType;
}


// 클라이언트 측으로 로그인 성공 유무 상태를 응답한다.
bool LOGIN_REQUEST::reponseLoginStatus(bool status)
{
	
	return true;
}