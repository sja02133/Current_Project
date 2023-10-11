#include "tcpSocket.h"

bool CCLIENT_CONTROL::Recv_Response(WCHAR* data, int& len, CLIENT_INFO& c_info)
{
	bool check = false;

	WCHAR resultValue = 0;

	WCHAR type = 0;
	int pos = 0;
	memcpy(&type, &data[0], 2);
	pos++;
	memcpy(&resultValue, &data[1], 1);
	if (resultValue == 0x01)
		check = true;
	else
		check = false;

	c_info.last_type = type;
	c_info.last_type_success = check;

	return check;
}

bool CCLIENT_CONTROL::Send_Fail(CLIENT_INFO& c_info, int code)
{
	Set_ErrorMsg(c_info, code);
	if (c_info.sendDataLength != 0) {
		// error msg 존재.
		MakeErrorExistMsg(code, c_info);
	}
	else {
		MakeErrorCode(code, c_info);
	}

	int check = send(c_info.cSock, (char*)c_info.sendData, c_info.sendDataLength * 2, 0);
	if (check > 0)
		return true;
	else
		return false;
	return true;
}

bool CCLIENT_CONTROL::Set_ErrorMsg(CLIENT_INFO& c_info, int code)
{
	CString msg = "";
	setlocale(LC_ALL, "");
	switch (code) {
	case ERROR_LOGIN_EXIST_SESSION:
		msg = _T("현재 로그인 중인 세션입니다.");
		break;
	case ERROR_LOGIN_ERROR_ID:
		msg = _T("아이디 혹은 비밀번호를 확인해주세요.");
		break;
	}

	memcpy(c_info.sendData, msg, msg.GetLength() * 2);
	c_info.sendDataLength = msg.GetLength();

	return true;
}

bool CCLIENT_CONTROL::MakeErrorCode(int code, CLIENT_INFO& c_info)
{
	WCHAR type = 'E';

	WCHAR* totalMsg = (WCHAR*)malloc(18);	// 9 => Type(1) + Length(4) + Code(4)

	int pos = 0;

	wmemset(totalMsg, 0, 18);
	memcpy(&totalMsg[pos], &type, 2);
	pos++;

	int size = 8;

	memcpy(&totalMsg[pos], &size, sizeof(int) * 2);
	pos += sizeof(int);

	memcpy(&totalMsg[pos], &code, sizeof(int) * 2);
	pos += sizeof(int);

	memcpy(c_info.sendData, totalMsg, pos * 2);

	free(totalMsg);

	return true;
}

bool CCLIENT_CONTROL::MakeErrorExistMsg(int code, CLIENT_INFO& c_info)
{
	WCHAR type = 'E';
	int beforeLength = c_info.sendDataLength;

	WCHAR* totalMsg = (WCHAR*)malloc((beforeLength + 9) * 2);	// 9 => Type(1) + Length(4) + Code(4)

	int pos = 0;

	wmemset(totalMsg, 0, beforeLength + 9);
	memcpy(&totalMsg[pos], &type, 2);
	pos++;

	int size = 8 + beforeLength;

	memcpy(&totalMsg[pos], &size, sizeof(int));
	pos += sizeof(int);

	memcpy(&totalMsg[pos], &code, sizeof(int));
	pos += sizeof(int);

	memcpy(&totalMsg[pos], c_info.sendData, beforeLength * 2);
	pos += beforeLength;

	memcpy(c_info.sendData, totalMsg, pos * 2);

	c_info.sendDataLength = pos;

	free(totalMsg);

	return true;
}