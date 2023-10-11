#pragma once

#include <Windows.h>
#include <sqlext.h>
#include <tchar.h>
#include <strsafe.h>
#include <string>
#include <list>

// TABLE LIST

// --

//wchar_t errorMsg[1024] = { 0 };

typedef struct struct_bindSqlColInfo {
	UINT th = 1;
	int sqlDataType;
	void* sqlData;
	int sizeOfSqlData;
	SQLLEN cLen;
}COLINFO;



class CODBC {
public:
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt;
	SQLRETURN retcode;
	SQLSMALLINT length;
	SQLINTEGER rec = 0, native;
	SQLWCHAR state[7], message[256];

	std::list<COLINFO> list_colInfo;

public:
	// ODBC �ڵ� �ʱ�ȭ
	bool AllocateHandles();
	
	// DBMS ����
	bool ConnectDataSource();
	
	// ������ ����
	bool ExecuteStatementDirect(SQLWCHAR* sql);

	// ���� �غ�
	bool PrepareStatement(SQLWCHAR* sql);

	// ���� ����
	bool ExecuteStatement();

	// ���� ���
	bool RetrieveResult();

	// �ڵ� ����
	void DisconnectDataSource();

};

// ���� ���� �����ϰ� �����ϴ� �Լ� �� �ڷᱸ��
class CUSTOM_ODBC_MSSQL {
	
};