#include "../../Common/GStreamer_MFC/_TCP_/tcpSocket.h"

#include "../Server_TCP.h"

#include "../../Common/GStreamer_MFC/byteControl/byteControl.h"

// 서버측 커스텀

bool CRECV_CONTROL_SERVER::Recv_DeleteLoginSessionAll()
{
	CString str = _T("TRUNCATE TABLE TB_LOGIN");

	COLEDB_HANDLER& db_connection_ = COLEDB_HANDLER::GetInstance();
	if (db_connection_.DeleteTBLogin(str))
		return true;
	else
		return false;
}

bool CRECV_CONTROL_SERVER::Recv_LoginResponse(WCHAR* data, int& len, SOCKET_INFO& c_info)
{
	int pos = 0;
	pos++;	// pass to first byte

	// check success
	if (data[pos] == 1)
		return true;
	else
		return false;
}

bool CRECV_CONTROL_SERVER::Recv_IDExist(WCHAR* data, int& len, SOCKET_INFO& c_info)
{
	c_info.checkResponse = true;

	CString id;

	int pos = 0;

	pos++;	// pass to first byte

	int length = 0;

	length = get_byteToInt((char*)&data[pos]);

	//memcpy(&length, &data[pos], sizeof(int));
	pos += sizeof(int);

	CString str = _T("");

	// parameter name id
	for (; data[pos] != ' '; pos++) {
		str += data[pos];
	}

	if (str != _T("id"))
		return false;

	str = _T("");
	pos++;

	// parameter value id
	for (; data[pos] != '\0'; pos++) {
		str += data[pos];
	}

	id = str;

	if (id.IsEmpty())
		return false;

	str = _T("");
	pos++;

	int rowCount = 0;

	str = _T("SELECT * FROM TB_USER WHERE user_id='") + id + _T("'");

	COLEDB_HANDLER& db_connection_ = COLEDB_HANDLER::GetInstance();
	//db_connection_.GetTBUser(rowCount, str);
	db_connection_.GetTBUserExist(rowCount, str);
	wmemset(&c_info.ID[0], 0, 100);
	memcpy(&c_info.ID[0], id.GetBuffer(), id.GetLength() * 2);
	printf("%ls, 아이디 중복 검사 결과 : ", c_info.ID);
	wmemset(&c_info.ID[0], 0, id.GetLength());
	if (rowCount > 0) {
		printf("중복 있음\n");
		return false;
	}
	printf("중복 없음\n");
	return true;
}

bool CRECV_CONTROL_SERVER::Recv_LoginRequest(WCHAR* data, int& len, SOCKET_INFO& c_info)
{
	//CString id, password;

	// 로그인 성공 시 => response(true) | 실패 시 => message(errorCode) OR response(false)

	c_info.checkResponse = true;

	CString id, password;

	int pos = 0;

	pos++;	// pass to first byte

	int length = 0;

	length = get_byteToInt((char*)&data[pos]);

	//memcpy(&length, &data[pos], sizeof(int));
	pos += sizeof(int);

	CString str = _T("");

	// parameter name id
	for (; data[pos] != ' '; pos++) {
		str += data[pos];
	}

	if (str != _T("id"))
		return false;

	str = _T("");
	pos++;

	// parameter value id
	for (; data[pos] != ' '; pos++) {
		str += data[pos];
	}

	id = str;

	if (id.IsEmpty())
		return false;

	str = _T("");
	pos++;

	// parameter name password
	for (; data[pos] != ' '; pos++) {
		str += data[pos];
	}

	if (str != _T("password"))
		return false;

	str = _T("");
	pos++;

	// parameter value password
	for (; data[pos] != '\0' && pos <= length - 1; pos++) {
		str += data[pos];
	}

	password = str;

	int rowCount = 0;

	int a = id.GetLength();

	memcpy(&c_info.ID[0], id.GetBuffer(), id.GetLength() * 2);
	//printf("%s 로그인 시도\n", c_info.ID);

	str = "SELECT * FROM TB_USER WHERE user_id='" + id + "' AND user_password='" + password + "'";
	//str = _T("SELECT user_id,user_password FROM TB_USER WHERE user_id='") + id + _T("' AND user_password='") + password + _T("'");

	COLEDB_HANDLER& db_connection_ = COLEDB_HANDLER::GetInstance();
	db_connection_.GetTBUserExist(rowCount, str);
	//db_connection_.SelectTBUser(str);

	if (rowCount == 0) {
		c_info.errorCode = ERROR_LOGIN_ERROR_ID;
		return false;
	}

	//printf("%s 유저 존재 확인\n", c_info.ID);

	int checkAlreadyExist = 0;

	str = _T("SELECT * FROM TB_LOGIN WHERE user_id='") + id + _T("'");
	//str = "SELECT * FROM TB_LOGIN WHERE user_id='" + id + "'";

	checkAlreadyExist = db_connection_.GetTBLoginExist(rowCount, str);

	if (checkAlreadyExist) {
		c_info.errorCode = ERROR_LOGIN_EXIST_SESSION;
		return false;
	}

	//if (checkAlreadyExist == 0)
	//	printf("%s 로그인 세션 없음\n", c_info.ID);
	//else
	//	printf("%s 로그인 세션 존재 혹은 조회 실패\n", c_info.ID);

	int checkInsert = 0;

	if (checkAlreadyExist == 0) {
		str = _T("INSERT INTO TB_LOGIN (user_id, user_login_st, user_lastlogin_dt) VALUES ('" + id +
			"',1,GETDATE())");
		checkInsert = db_connection_.InsertTBLogin(str);
	}

	//printf("%s 로그인 세션 생성\n", c_info.ID);

	if (rowCount > 0 && checkInsert) {
		// 로그인 성공 시엔 SOCKET_INFO& c_info 내의 ID에 이 함수 내의 id를 작성할 것.
		printf("%s 로그인!\n", c_info.ID);
		return true;
	}
	else if (rowCount > 0 && checkAlreadyExist) {
		printf("%s은 로그인 세션을 가지고 있습니다.\n", c_info.ID);
		wmemset(&c_info.ID[0], 0, id.GetLength());
		return false;
	}
	else {
		printf("%s 로그인 실패!\n", c_info.ID);
		wmemset(&c_info.ID[0], 0, id.GetLength());
		return false;
	}
}

bool CRECV_CONTROL_SERVER::Recv_MembershipJoin(WCHAR* data, int& len, SOCKET_INFO& c_info)
{
	CString id, password;

	int pos = 0;

	pos++;	// pass to first byte

	int length = 0;

	length = get_byteToInt((char*)&data[pos]);

	//memcpy(&length, &data[pos], sizeof(int));
	pos += sizeof(int);

	CString str = _T("");

	// parameter name id
	for (; data[pos] != ' '; pos++) {
		str += data[pos];
	}

	if (str != _T("id"))
		return false;

	str = _T("");
	pos++;

	// parameter value id
	for (; data[pos] != ' '; pos++) {
		str += data[pos];
	}

	id = str;

	if (id.IsEmpty())
		return false;

	str = _T("");
	pos++;

	// parameter name password
	for (; data[pos] != ' '; pos++) {
		str += data[pos];
	}

	if (str != _T("password"))
		return false;

	str = _T("");
	pos++;

	// parameter value password
	for (; data[pos] != '\0' && pos <= length - 1; pos++) {
		str += data[pos];
	}

	password = str;

	int rowCount = 0;

	str = _T("INSERT INTO TB_USER (user_id, user_password, user_use_st,user_created_dt) VALUES ('") + id +
		_T("','") + password + _T("',1,getdate())");

	COLEDB_HANDLER& db_connection_ = COLEDB_HANDLER::GetInstance();
	int check = db_connection_.InsertTBUser(str);

	if (check == 1) {
		memcpy(&c_info.ID[0], id.GetBuffer(), id.GetLength() * 2);
		printf("%s 회원가입 성공!!\n", c_info.ID);
		return true;
	}
	else
		return false;
}

bool CRECV_CONTROL_SERVER::Recv_LoginOutRequest(WCHAR* data, int& len, SOCKET_INFO& c_info)
{
	CString id;

	int pos = 0;

	pos++;	// pass to first byte

	int length = 0;

	length = get_byteToInt((char*)&data[pos]);

	//memcpy(&length, &data[pos], sizeof(int));
	pos += sizeof(int);

	CString str = "";

	// parameter name id
	for (; data[pos] != ' '; pos++) {
		str += data[pos];
	}

	if (str != _T("id"))
		return false;

	str = _T("");
	pos++;

	// parameter value id
	for (; data[pos] != '\0'; pos++) {
		str += data[pos];
	}

	id = str;

	if (id.IsEmpty())
		return false;

	str = _T("");
	pos++;

	int rowCount = 0;

	str = _T("DELETE FROM TB_LOGIN WHERE user_id='") + id + _T("'");

	COLEDB_HANDLER& db_connection_ = COLEDB_HANDLER::GetInstance();
	if (db_connection_.DeleteTBLogin(str))
		return true;
	else
		return false;
}

bool CRECV_CONTROL_SERVER::Recv_AlreadyLoginSessionExist(WCHAR* data, int& len, SOCKET_INFO& c_info)
{
	CString id;

	int pos = 0;

	pos++;	// pass to first byte

	int length = 0;

	length = get_byteToInt((char*)&data[pos]);

	//memcpy(&length, &data[pos], sizeof(int));
	pos += sizeof(int);

	CString str = _T("");

	// parameter name id
	for (; data[pos] != ' '; pos++) {
		str += data[pos];
	}

	if (str != _T("id"))
		return false;

	str = _T("");
	pos++;

	// parameter value id
	for (; data[pos] != '\0'; pos++) {
		str += data[pos];
	}

	id = str;

	if (id.IsEmpty())
		return false;

	str = _T("");
	pos++;

	int rowCount = 0;

	str = _T("SELECT * FROM TB_LOGIN WHERE user_id='") + id + _T("'");

	COLEDB_HANDLER& db_connection_ = COLEDB_HANDLER::GetInstance();
	db_connection_.GetTBLoginExist(rowCount, str);
	if (rowCount > 0) {
		printf("%s는 현재 로그인 세션에 존재합니다.\n", id.GetBuffer());
		return false;
	}
	printf("%s, 환영합니다.\n", id.GetBuffer());
	return true;
}

bool CRECV_CONTROL_SERVER::Recv_LoginSessionList(WCHAR* data, int& len, SOCKET_INFO& c_info)
{
	CString str = _T("SELECT TB_LOGIN.USER_ID FROM TB_LOGIN");
	COLEDB_HANDLER& db_connection_ = COLEDB_HANDLER::GetInstance();
	std::list<QUERY_RESULT> result = db_connection_.SelectTBLogin(str);

	if (result.size() == 0) {
		return false;
	}

	if (result.front().type == -1) {
		// error
		return false;
	}

	memset(c_info.sendData, 0, MAX_WCHAR_SIZE);

	WCHAR type = 'S';

	int pos = 0;

	memset(&c_info.sendData[pos], type, 1);
	pos++;	// type

	int length = 0;

	short columnNameCount = 0;
	WCHAR* columnName = 0;

	CString name = _T("");
	auto iter = result.begin();

	pos += sizeof(int);	// length 공간 먼저 할당, 값은 추후에 변수 'length'에 쌓이는 값을 활용

	std::list<CSERVER_CONTROL::COLUMN_DATA> columnName_list;
	CSERVER_CONTROL::COLUMN_DATA tempData;

	// 만약 현재 최초 접속자이면, type(1) + length(4) + 0(2) 을 보내자.
	if (iter->result.size() == 1) {
		memset(c_info.sendData, 0, MAX_WCHAR_SIZE);
		pos = 0;
		memcpy(&c_info.sendData[pos], &type, 1);
		pos++;

		length = 7;

		memcpy(&c_info.sendData[pos], &length, sizeof(int));				pos += sizeof(int);
		memcpy(&c_info.sendData[pos], &columnNameCount, sizeof(short));		pos += sizeof(short);

		c_info.checkResponse = false;
		c_info.sendDataLength = pos;
		return true;
	}

	// Column Name
	for (auto iter2 = iter->result.begin(); iter2 != iter->result.end(); iter2++) {
		if (iter2 == iter->result.begin())
			columnNameCount = iter->result.size();
		// column name list / count done.

		tempData.db_type = iter2->dbType;
		tempData.len = iter2->resultString.GetLength();
		tempData.data = (WCHAR*)malloc(tempData.len * 2);
		memcpy(tempData.data, iter2->resultString.GetBuffer(), tempData.len * 2);
		length += tempData.len;
		columnName_list.push_back(tempData);
	}
	// column name list done

	short columnValueCount = 0;
	WCHAR* columnValue = 0;

	iter++;
	std::list<CSERVER_CONTROL::COLUMN_DATA> columnValue_list;
	CSERVER_CONTROL::COLUMN_DATA tempData2;
	// Column Name
	for (auto iter2 = iter->result.begin(); iter2 != iter->result.end(); iter2++) {
		if (iter2 == iter->result.begin())
			columnNameCount = iter->result.size();
		// column name list / count done.

		tempData2.db_type = iter2->dbType;
		tempData2.len = iter2->resultString.GetLength();
		tempData2.data = (WCHAR*)malloc(tempData2.len * 2);
		memcpy(tempData2.data, iter2->resultString.GetBuffer(), tempData2.len * 2);
		length += tempData2.len;
		columnValue_list.push_back(tempData2);
	}
	// column name list done

	memcpy(&c_info.sendData[pos], &length, sizeof(int));
	pos += sizeof(int);

	memcpy(&c_info.sendData[pos], &columnNameCount, sizeof(short));
	pos += sizeof(short);

	for (auto iter = columnName_list.begin(); iter != columnName_list.end(); iter++) {
		memcpy(&c_info.sendData[pos], &iter->db_type, sizeof(byte));
		pos += sizeof(byte);
		memcpy(&c_info.sendData[pos], &iter->len, sizeof(int));
		pos += sizeof(int);
		memcpy(&c_info.sendData[pos], &iter->data[0], iter->len * 2);
		pos += iter->len * 2;
	}

	memcpy(&c_info.sendData[pos], &columnValueCount, sizeof(short));
	pos += sizeof(short);

	for (auto iter = columnValue_list.begin(); iter != columnValue_list.end(); iter++) {
		memcpy(&c_info.sendData[pos], &iter->len, sizeof(int));
		pos += sizeof(int);
		memcpy(&c_info.sendData[pos], &iter->data[0], iter->len * 2);
		pos += iter->len * 2;
	}
	c_info.sendDataLength = pos;

	c_info.checkResponse = false;

	return true;
}