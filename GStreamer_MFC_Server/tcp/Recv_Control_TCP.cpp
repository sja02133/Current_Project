#include "../../Common/GStreamer_MFC/_TCP_/tcpSocket.h"

#include "../Server_TCP.h"

#include "../../Common/GStreamer_MFC/byteControl/byteControl.h"

#define SAVE_FILE_DIRECTORY _T(".\\chatting_file_storage")
#define SAVE_FILE_PATH _T(".\\chatting_file_storage\\")

// ������ Ŀ����

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
	printf("%ls, ���̵� �ߺ� �˻� ��� : ", c_info.ID);
	wmemset(&c_info.ID[0], 0, id.GetLength());
	if (rowCount > 0) {
		printf("�ߺ� ����\n");
		return false;
	}
	printf("�ߺ� ����\n");
	return true;
}

bool CRECV_CONTROL_SERVER::Recv_LoginRequest(WCHAR* data, int& len, SOCKET_INFO& c_info)
{
	//CString id, password;

	// �α��� ���� �� => response(true) | ���� �� => message(errorCode) OR response(false)

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

	//memcpy(&c_info.ID[0], id.GetBuffer(), id.GetLength() * 2);
	//printf("%s �α��� �õ�\n", c_info.ID);

	str = "SELECT * FROM TB_USER WHERE user_id='" + id + "' AND user_password='" + password + "'";
	//str = _T("SELECT user_id,user_password FROM TB_USER WHERE user_id='") + id + _T("' AND user_password='") + password + _T("'");

	COLEDB_HANDLER& db_connection_ = COLEDB_HANDLER::GetInstance();
	db_connection_.GetTBUserExist(rowCount, str);
	//db_connection_.SelectTBUser(str);

	if (rowCount == 0) {
		c_info.errorCode = ERROR_LOGIN_ERROR_ID;
		return false;
	}

	//printf("%s ���� ���� Ȯ��\n", c_info.ID);

	int checkAlreadyExist = 0;

	str = _T("SELECT * FROM TB_LOGIN WHERE user_id='") + id + _T("'");
	//str = "SELECT * FROM TB_LOGIN WHERE user_id='" + id + "'";

	checkAlreadyExist = db_connection_.GetTBLoginExist(rowCount, str);

	if (checkAlreadyExist) {
		c_info.errorCode = ERROR_LOGIN_EXIST_SESSION;
		return false;
	}

	//if (checkAlreadyExist == 0)
	//	printf("%s �α��� ���� ����\n", c_info.ID);
	//else
	//	printf("%s �α��� ���� ���� Ȥ�� ��ȸ ����\n", c_info.ID);

	int checkInsert = 0;

	if (checkAlreadyExist == 0) {
		str = _T("BEGIN SET NOCOUNT ON INSERT INTO TB_LOGIN (user_id, user_login_st, user_lastlogin_dt) VALUES ('" + id +
			"',1,GETDATE()) END");
		checkInsert = db_connection_.InsertTBLogin(str);

		// 2023.10.20
		// COMMIT �߰�..
		if (checkInsert != 0) {
			int checkCommit = db_connection_.Commit();
		}
	}

	//printf("%s �α��� ���� ����\n", c_info.ID);

	if (rowCount > 0 && checkInsert) {
		// �α��� ���� �ÿ� SOCKET_INFO& c_info ���� ID�� �� �Լ� ���� id�� �ۼ��� ��.
		memcpy(&c_info.ID[0], id.GetBuffer(), id.GetLength() * 2);
		printf("%ws �α���!\n", c_info.ID);
		return true;
	}
	else if (rowCount > 0 && checkAlreadyExist) {
		printf("%ws�� �α��� ������ ������ �ֽ��ϴ�.\n", c_info.ID);
		wmemset(&c_info.ID[0], 0, id.GetLength());
		return false;
	}
	else {
		printf("%ws �α��� ����!\n", c_info.ID);
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
		printf("%s ȸ������ ����!!\n", c_info.ID);
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
		printf("%s�� ���� �α��� ���ǿ� �����մϴ�.\n", id.GetBuffer());
		return false;
	}
	printf("%s, ȯ���մϴ�.\n", id.GetBuffer());
	return true;
}

bool CRECV_CONTROL_SERVER::Recv_LoginSessionList(WCHAR* data, int& len, SOCKET_INFO& c_info)
{
	printf("����\n");

	CString str = _T("SELECT TB_LOGIN.USER_ID FROM TB_LOGIN");
	COLEDB_HANDLER& db_connection_ = COLEDB_HANDLER::GetInstance();
	std::list<QUERY_RESULT> col_name, col_value;
	int checkSelect = db_connection_.SelectTBLogin(str,col_name,col_value);

	if (col_name.size() == 0) {
		return false;
	}

	if (col_name.front().type == -1) {
		// error
		return false;
	}

	memset(c_info.sendData, 0, MAX_WCHAR_SIZE);

	WCHAR type = 'S';

	int pos = 0;

	memset(&c_info.sendData[pos], type, 1);
	pos++;	// type

	// type done

	int length = 0;

	short columnNameCount = 0;
	WCHAR* columnName = 0;

	CString name = _T("");
	//auto iter = col_name.begin();

	//pos += sizeof(int);	// length ���� ���� �Ҵ�, ���� ���Ŀ� ���� 'length'�� ���̴� ���� Ȱ��

	//std::list<CSERVER_CONTROL::COLUMN_DATA> columnName_list;
	//std::list<RESULT_DATA> columnName_list;
	//CSERVER_CONTROL::COLUMN_DATA tempData;

	// ���� ���� ���� �������̸�, type(1) + length(4) + 0(2) �� ������.
	// 2023.10.20 -> ���� �������� ���� �ּ� ó��
	//if (col_name.size() == 1) {
	//	memset(c_info.sendData, 0, MAX_WCHAR_SIZE);
	//	pos = 0;
	//	memcpy(&c_info.sendData[pos], &type, 1);
	//	pos++;

	//	length = 7;

	//	memcpy(&c_info.sendData[pos], &length, sizeof(int));				pos += sizeof(int);
	//	memcpy(&c_info.sendData[pos], &columnNameCount, sizeof(short));		pos += sizeof(short);

	//	c_info.checkResponse = false;
	//	c_info.sendDataLength = pos;
	//	return true;
	//}

	// Column Name
	//for (auto iter = col_name.begin()->result.begin(); iter != col_name.begin()->result.end(); iter++) {
	//	if (iter == col_name.begin()->result.begin()) {
	//		columnNameCount = col_name.size();
	//		length += sizeof(short);
	//	}
	//	// column name list / count done.
	//	RESULT_DATA r_data;
	//	r_data.dbType = iter->dbType;
	//	r_data.resultString = iter->resultString;
	//	columnName_list.push_back(r_data);
	//}
	//// column name list done

	//short columnValueCount = 0;
	//WCHAR* columnValue = 0;

	//iter++;
	//std::list<CSERVER_CONTROL::COLUMN_DATA> columnValue_list;
	//CSERVER_CONTROL::COLUMN_DATA tempData2;
	//// Column Name
	//for (auto iter2 = iter->result.begin(); iter2 != iter->result.end(); iter2++) {
	//	if (iter2 == iter->result.begin()) {
	//		columnValueCount = iter->result.size();
	//		length += sizeof(short);
	//	}
	//	// column name list / count done.

	//	tempData2.db_type = iter2->dbType;
	//	tempData2.len = iter2->resultString.GetLength();
	//	tempData2.data = (WCHAR*)malloc(tempData2.len * 2);
	//	memcpy(tempData2.data, iter2->resultString.GetBuffer(), tempData2.len * 2);
	//	length += tempData2.len;
	//	columnValue_list.push_back(tempData2);
	//}
	// column name list done

	columnNameCount = col_name.size();

	// length ���..
	// type(1) + total length(4) + Column Name List(����)[ count(����,2) * { DB Type(1) + Length(4) + Name(Length) } ]
	// + Column Name Value [ Count(����,2) * { Length(4) + Value(Length) } ]
	// ���� pos�� total Length�� ����ϴ°� ��������?
	// 

	// ���� 0���� �̷���� 4byte �ڸ��� ���� �Ҵ� -> ���� �̰��� pos�� ���̶� �����Ѵ�.
	memcpy(&c_info.sendData[pos], &length, sizeof(int));
	pos += sizeof(int);

	memcpy(&c_info.sendData[pos], &columnNameCount, sizeof(short));
	pos += sizeof(short);



	//for (auto iter = col_name.begin()->result.begin(); iter != col_name.begin()->result.end(); iter++) {
	//	memcpy(&c_info.sendData[pos], &iter->dbType, sizeof(byte));
	//	pos += sizeof(byte);
	//	int resultStringLength = iter->resultString.GetLength();
	//	memcpy(&c_info.sendData[pos], &resultStringLength, sizeof(int));
	//	pos += sizeof(int);
	//	memcpy(&c_info.sendData[pos], iter->resultString.GetBuffer(), resultStringLength*2);
	//	pos += resultStringLength;
	//}

	for (auto iter = col_name.begin(); iter != col_name.end(); iter++) {
		auto col_name_iter = iter->result.begin();
		memcpy(&c_info.sendData[pos], &col_name_iter->dbType, sizeof(byte));
		pos += sizeof(byte);
		int resultStringLength = col_name_iter->resultString.GetLength();
		memcpy(&c_info.sendData[pos], &resultStringLength, sizeof(int));
		pos += sizeof(int);
		memcpy(&c_info.sendData[pos], col_name_iter->resultString.GetBuffer(), resultStringLength * 2);
		pos += resultStringLength;
	}

	short columnValueCount = col_value.size();

	memcpy(&c_info.sendData[pos], &columnValueCount, sizeof(short));
	pos += sizeof(short);

	

	for (auto iter = col_value.begin(); iter != col_value.end(); iter++) {
		auto col_value_iter = iter->result.begin();
		int resultStringLength = col_value_iter->resultString.GetLength();
		memcpy(&c_info.sendData[pos], &resultStringLength, sizeof(int));
		pos += sizeof(int);
		memcpy(&c_info.sendData[pos], col_value_iter->resultString.GetBuffer(), resultStringLength * 2);
		pos += resultStringLength;
	}
	c_info.sendDataLength = pos;

	memcpy(&c_info.sendData[1], &pos, sizeof(int));
	c_info.checkResponse = false;

	return true;
}

bool CRECV_CONTROL_SERVER::Recv_Chatting_Something(WCHAR* data, int& len, SOCKET_INFO& c_info)
{
	// ���� ���� ä�� Ÿ�Կ� ���� �ٸ� ó���� �ؾ��Ѵ�.

	// message
	int pos = 0;
	
	pos++;	//pass to type

	pos += sizeof(int);	// total length

	pos += sizeof(int);	// message info length

	switch (data[pos]) {
	case _T('M'):
	{
		// just message
		Recv_Chatting_Message(data, len, c_info);
		break;
	}
	case _T('F'):
	{
		// file
		Recv_Chatting_File(data, len, c_info);
		break;
	}
	default:
	{
		// ???
		return false;
	}
	}
	return true;
}

bool CRECV_CONTROL_SERVER::Recv_Chatting_Message(WCHAR* data, int& len, SOCKET_INFO& c_info)
{
	// ä�� �޽��� ���� ������ �ϴ� �ٷ� �ٸ� Ŭ���̾�Ʈ�鿡�� �״�� ��������..

	for (auto iter = c_info.pTCP_SOCKET->socket_map.begin(); iter != c_info.pTCP_SOCKET->socket_map.end(); iter++) {
		send(iter->second.cSock,(char*)data,len,0);
	}
	return true;
}

bool CRECV_CONTROL_SERVER::Recv_Chatting_File(WCHAR* data, int& len, SOCKET_INFO& c_info)
{
	// ���� ������ �ǽ��Ͽ��� �Ѵ�.

	// ���� ��� : ../chatting_file_storage �� �ϴ� ��η� ���̵� ���� ������ �����Ѵ�.

	int pos = 0;

	pos++;	//pass to type

	int totalLength = 0;
	memcpy(&totalLength, &data[pos], sizeof(int));
	pos += sizeof(int);	// total length
	
	// message info
	//message info length
	int messageInfoTotalLength = 0;
	memcpy(&messageInfoTotalLength, &data[pos], sizeof(int));
	pos += sizeof(int);	// message info length

	// message type
	WCHAR m_type = 0;
	memcpy(&m_type, &data[pos], sizeof(WCHAR));
	pos += 1;

	// send id length
	short sendIDLength = 0;
	memcpy(&sendIDLength, &data[pos], sizeof(short));
	pos += sizeof(short);

	//send id
	CString id = _T("");
	for (short i = 0; i < sendIDLength; i++,pos++) {
		id += data[pos];
	}

	pos++;	//pass to \0

	// file count
	byte fileCount = 0;
	memcpy(&fileCount, &data[pos], sizeof(byte));
	pos += sizeof(byte);

	// Ȯ���ڸ�
	std::list<CString> extension_list;
	for (byte i = 0; i < fileCount; i++,pos++) {
		CString extension = _T("");
		while (data[pos] != '\0') {
			extension += data[pos];
			pos++;
		}
		extension_list.push_back(extension);
	}

	//pos++;	//pass to \0
	std::list<CString> fileName_list;
	for (int i = 0; i < fileCount; i++) {
		// file name length
		short fileNameLength = 0;
		memcpy(&fileNameLength, &data[pos], sizeof(short));
		pos += sizeof(short);
		// file name
		int j = 0;
		CString fileName = _T("");
		for (j = 0; j < fileNameLength; j++) {
			fileName += data[pos+j];
		}
		fileName_list.push_back(fileName);
		pos += j;
	}

	// Data
	// Data size

	//if (pos != totalLength) {
	//	// error....
	//	printf("File ���ε� ����..\n");
	//	return false;
	//}
	

	// ���� ��η� �̵��ؼ� �ش� ������ ������ ������ ���̵��� ���͸��� �ִ��� �˻��ϰ� ������ ����.
	if (GetFileAttributesW(SAVE_FILE_DIRECTORY) == 0xFFFFFFFF) {
		// ���丮�� ������..
		if (!CreateDirectoryW(SAVE_FILE_DIRECTORY, NULL)) {
			DWORD error = GetLastError();
			if (error == ERROR_ALREADY_EXISTS)
				printf("������ �̹� ����\n");
			else if (error == ERROR_PATH_NOT_FOUND)
				printf("�ϳ� �̻��� �߰� ���͸� �������� �ʽ��ϴ�. �� �Լ��� ��ο� ���� ���͸��� ����ϴ�.\n");
			int kkk = 0;
		}
	}
	
	CString save_id_path = SAVE_FILE_PATH + id;
	if (GetFileAttributesW(save_id_path) == 0xFFFFFFFF) {
		// ���丮�� ������..
		if (!CreateDirectoryW(save_id_path.GetBuffer(), NULL)) {
			DWORD error = GetLastError();
			if (error == ERROR_ALREADY_EXISTS)
				printf("������ �̹� ����\n");
			else if (error == ERROR_PATH_NOT_FOUND)
				printf("�ϳ� �̻��� �߰� ���͸� �������� �ʽ��ϴ�. �� �Լ��� ��ο� ���� ���͸��� ����ϴ�.\n");
			int kkk = 0;
		}
	}
	save_id_path += _T("\\");

	// ���� �����ͺ� ���� �Ϸ�...

	std::list<CString> fileNameWithExtension_list;

	for (auto iter = fileName_list.begin(),iter2 = extension_list.begin(); 
		(iter != fileName_list.end()) && (iter2 != extension_list.end()) ; iter++,iter2++) {
		// ���� ���� ��ƾ..
		CString fileNameWithExtension = iter->GetBuffer();
		fileNameWithExtension += _T('.');
		fileNameWithExtension += iter2->GetBuffer();
		fileNameWithExtension = save_id_path + fileNameWithExtension;
		fileNameWithExtension_list.push_back(fileNameWithExtension);
	}

	for (auto iter = fileNameWithExtension_list.begin(); iter != fileNameWithExtension_list.end(); iter++) {
		long dataSize = 0;
		memcpy(&dataSize, &data[pos], sizeof(long));
		//pos += sizeof(long);
		int file_pos = sizeof(long);
		char* file_data = (char*)malloc(dataSize+sizeof(long));
		memset(&file_data[0], 0, dataSize);
		memcpy(&file_data[0], (char*)&data[pos], (size_t)(dataSize+sizeof(long)));
		
		if (dataSize % 2 != 0) {
			// ���� ����� Ȧ��
			pos += ((dataSize+1) + sizeof(long))/2;
		}
		else {
			// ���� ����� ¦��
			pos += (dataSize + sizeof(long)) / 2;
		}
		
		FILE* pFile;
		_wfopen_s(&pFile, iter->GetBuffer(), _T("w"));
		fwrite(&file_data[file_pos], 1, dataSize, pFile);
		fclose(pFile);

		free(file_data);
	}
	

	// 2023.12.01

	// Ŭ���̾�Ʈ <-> ���� ���� ������ �ۼ��� ���� ��, Ŭ���̾�Ʈ������ ���ο� ���� ���� ���� ������ ���� ����ϴ� ������� �ٲ�� �Ѵ�.
	// 

	// --





	return true;
}