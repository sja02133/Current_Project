
#include "login.h"

// ���� ��Ŷ���� �α��� ������ �����´�.
bool LOGIN_REQUEST::getLoginInfo(unsigned char* data)
{

	return true;
}

// Ŭ���̾�Ʈ �����κ��� �� �α��� ������ �����Ѵ�.
uint32_t LOGIN_REQUEST::checkLoginInfo()
{
	uint32_t returnType = 0;

	// ���̵� ���� ( DB������ �ߺ��˻�. )
	
	// ��й�ȣ ���� ( ���� : Ư������(!,@,#) <- 3�� �߿� �ϳ� ���� + ���ĺ� + ����, ���� �������)
	if (this->password.find('!', 0)) {

	}



	return returnType;
}


// Ŭ���̾�Ʈ ������ �α��� ���� ���� ���¸� �����Ѵ�.
bool LOGIN_REQUEST::reponseLoginStatus(bool status)
{
	
	return true;
}