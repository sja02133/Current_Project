
#include "PODBC.h"

// ODBC 핸들 초기화
bool CODBC::AllocateHandles()
{
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		// OBCD 드라이버 버전 명시
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				// SUCESS
				return TRUE;
			}
			else {
				SQLGetDiagRec(SQL_HANDLE_DBC, hdbc, ++rec, state, &native, message, sizeof(message), &length);
				//StringCbPrintf(errorMsg,1024 ,_T("%s : %ld : %ld : %s"), state, rec, native, message);
			}
		}
		else {
			SQLGetDiagRec(SQL_HANDLE_DBC, hdbc, ++rec, state, &native, message, sizeof(message), &length);
			//StringCbPrintf(errorMsg, 1024, _T("%s : %ld : %ld : %s"), state, rec, native, message);
		}
	}
	else {
		SQLGetDiagRec(SQL_HANDLE_DBC, hdbc, ++rec, state, &native, message, sizeof(message), &length);
		//StringCbPrintf(errorMsg, 1024, _T("%s : %ld : %ld : %s"), state, rec, native, message);
	}
	return FALSE;
}

// DBMS 접속
bool CODBC::ConnectDataSource()
{
	retcode = SQLConnect(hdbc, (SQLWCHAR*)"GStreamer_DB_13", SQL_NTS, (SQLWCHAR*)"doyuep", SQL_NTS, (SQLWCHAR*)"k4dev))))", SQL_NTS);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		return true;
	}
	else {
		SQLGetDiagRec(SQL_HANDLE_DBC, hdbc, ++rec, state, &native, message, sizeof(message), &length);
		
		//StringCbPrintf(errorMsg, 1024, _T("%s : %ld : %ld : %s"), state, rec, native, message);
	}
	return false;
}

// 쿼리문 실행
bool CODBC::ExecuteStatementDirect(SQLWCHAR* sql)
{
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
		//StringCbPrintf(errorMsg, 1024, _T("Query Seuccess"));
	}

	else {
		SQLGetDiagRec(SQL_HANDLE_DBC, hdbc, ++rec, state, &native, message, sizeof(message), &length);
		//StringCbPrintf(errorMsg, 1024, _T("%s : %ld : %ld : %s"), state, rec, native, message);
	}

	retcode = SQLExecDirect(hstmt, sql, SQL_NTS);

	if (retcode == SQL_SUCCESS) {
		//StringCbPrintf(errorMsg, 1024, _T("Query Seuccess"));
	}

	else {
		SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, ++rec, state, &native, message, sizeof(message), &length);
		//StringCbPrintf(errorMsg, 1024, _T("%s : %ld : %ld : %s"), state, rec, native, message);
	}

	return true;
}

bool CODBC::PrepareStatement(SQLWCHAR* sql) 
{
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
		//StringCbPrintf(errorMsg, 1024, _T("Query Seuccess"));
	}

	else {
		SQLGetDiagRec(SQL_HANDLE_DBC, hdbc, ++rec, state, &native, message, sizeof(message), &length);
		//StringCbPrintf(errorMsg, 1024, _T("%s : %ld : %ld : %s"), state, rec, native, message);
	}

	retcode = SQLPrepare(hstmt, sql, SQL_NTS);

	if (retcode == SQL_SUCCESS) {
		//StringCbPrintf(errorMsg, 1024, _T("Query Seuccess"));
	}

	else {
		SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, ++rec, state, &native, message, sizeof(message), &length);
		//StringCbPrintf(errorMsg, 1024, _T("%s : %ld : %ld : %s"), state, rec, native, message);
	}

	return true;
}

// 쿼리 실행
bool CODBC::ExecuteStatement()
{
	retcode = SQLExecute(hstmt);

	if (retcode == SQL_SUCCESS) {
		//StringCbPrintf(errorMsg, 1024, _T("Query Execute Seuccess"));
	}

	else {
		SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, ++rec, state, &native, message, sizeof(message), &length);
		//StringCbPrintf(errorMsg, 1024, _T("%s : %ld : %ld : %s"), state, rec, native, message);
	}
	return true;
}

// 쿼리 결과
bool CODBC::RetrieveResult()
{
	if (list_colInfo.size() == 0)
		return false;

	for (auto iter = list_colInfo.begin(); iter != list_colInfo.end(); iter++) {
		SQLBindCol(hstmt, iter->th,iter->sqlDataType,iter->sqlData,iter->sizeOfSqlData,&iter->cLen);
	}

	std::string queryResultString;

	do {
		retcode = SQLFetch(hstmt);

		// 쿼리 데이터 불러옴..
		int kkk = 0;

	} while (retcode != SQL_NO_DATA);

	return true;
}

void CODBC::DisconnectDataSource() {
	if (hstmt) {
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		hstmt = NULL;
	}

	SQLDisconnect(hdbc);

	if (hdbc) {
		SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
		hdbc = NULL;
	}

	if (henv) {
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
		henv = NULL;
	}

}
