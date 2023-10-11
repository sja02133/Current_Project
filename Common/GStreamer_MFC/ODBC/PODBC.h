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
	// ODBC 핸들 초기화
	bool AllocateHandles();
	
	// DBMS 접속
	bool ConnectDataSource();
	
	// 쿼리문 실행
	bool ExecuteStatementDirect(SQLWCHAR* sql);

	// 쿼리 준비
	bool PrepareStatement(SQLWCHAR* sql);

	// 쿼리 실행
	bool ExecuteStatement();

	// 쿼리 결과
	bool RetrieveResult();

	// 핸들 해제
	void DisconnectDataSource();

};

// 내가 만든 서버하고 소통하는 함수 및 자료구조
class CUSTOM_ODBC_MSSQL {
	
};