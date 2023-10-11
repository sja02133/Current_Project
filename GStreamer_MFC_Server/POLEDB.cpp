
#include "POLEDB.h"
#include "DBStructure.h"
#include <atlstr.h>

int GetNoOfTable(CString tableStr) {
	int no = 0;
	if (tableStr.Find(_T("TB_LOGIN")) >= 0) {
		no = TB_LOGIN;
	}
	else if (tableStr.Find(_T("TB_USER")) >= 0) {
		no = TB_USER;
	}

	// ....

	return no;
}

COLEDB::COLEDB() {
	hr = CoInitialize(NULL);
	if (FAILED(hr) || hr == S_FALSE) {
		//error
		CoUninitialize();
		CoInitialize(NULL);
	}
	hr = CLSIDFromProgID(OLESTR("SQLOLEDB.1"), &cls_id);
	if (FAILED(hr)) {
		//error
		int k = 0;
	}
}

bool COLEDB::InitCDBPropSet()
{
	CDBPropSet tempDBinit(DBPROPSET_DBINIT);
	bool checkAddProperty = false;
	checkAddProperty = tempDBinit.AddProperty(DBPROP_INIT_PROMPT, (SHORT)4);
	checkAddProperty = tempDBinit.AddProperty(DBPROP_INIT_PROVIDERSTRING, OLESTR("PROVIDER=SQLOLEDB.1;SERVER=127.0.0.1,2016;UID=doyuep;PWD=k4dev))));DATABASE=GStreamer_Server"));
	checkAddProperty = tempDBinit.AddProperty(DBPROP_INIT_LCID, (LONG)1043);

	if (!checkAddProperty) {
		return false;
	}

	this->dbinit = tempDBinit;

	return true;
}

bool COLEDB::OpenCDataSource()
{
	hr = ds.Open(cls_id, &dbinit);
	if (FAILED(hr)) {
		return false;
	}
	return true;
}

bool COLEDB::StartSession()
{
	hr = session.Open(ds);
	if (FAILED(hr)) {
		return false;
	}
	return true;
}

CSession COLEDB::GetSession()
{
	return session;
}

bool COLEDB::Connect()
{
	bool check = false;
	check = InitCDBPropSet();
	check = OpenCDataSource();
	check = StartSession();
	return check;
}

int COLEDB::ReadDB(CString strSQL,int table_no)
{
	int no = GetNoOfTable(strSQL);

	if (no == TB_LOGIN) {
		
	}
	else if (no == TB_USER) {
		CCommand<CAccessor<CTB_USER>, CRowset, CMultipleResults> resultList;
	}


	return 0;
}
int COLEDB::WriteDB(CString strSQL)
{
	return 0;
}


//DB handler
COLEDB_HANDLER::COLEDB_HANDLER()
{
	Initialize();
}
COLEDB_HANDLER::~COLEDB_HANDLER()
{
}

COLEDB_HANDLER& COLEDB_HANDLER::GetInstance()
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	static COLEDB_HANDLER instance_;
	return instance_;
}

void COLEDB_HANDLER::Initialize()
{
	db_connection = std::make_unique<COLEDB>();
	bool is_success = db_connection->Connect();
	if (is_success == false)
		DebugBreak();

	// func_list_ 배열에 함수 포인터를 할당.
	func_list_[TB_LOGIN] = &COLEDB_HANDLER::GetTBLogin;
	func_list_[TB_USER] = &COLEDB_HANDLER::GetTBUser;

	return;
}

int COLEDB_HANDLER::DBProcess(TABLE_NO tb_no, int& row_count, CString query)
{
	return (this->*func_list_[tb_no])(row_count,query);
}

int COLEDB_HANDLER::GetTBLogin(int& row_count, CString query)
{
	CCommand<CAccessor<CTB_LOGIN>,CRowset,CMultipleResults> cmd;
	
	HRESULT hr = cmd.Open(db_connection->GetSession(), query.GetBuffer());
	if (FAILED(hr))
		return -1;

	while (SUCCEEDED(hr) && hr != DB_S_ENDOFROWSET) {
		hr = cmd.MoveNext();
	}

	return 0;
}


int COLEDB_HANDLER::GetTBUser(int& row_count, CString query)
{
	CCommand<CAccessor<CTB_USER>, CRowset, CMultipleResults> cmd;

	HRESULT hr = cmd.Open(db_connection->GetSession(), query.GetBuffer());
	if (FAILED(hr))
		return -1;

	while (cmd.MoveNext() == S_OK) {
		++row_count;
	}
	return 0;
}

int COLEDB_HANDLER::GetTBUserExist(int& row_count, CString query)
{
	CCommand<CAccessor<CTB_USER>, CRowset, CMultipleResults> cmd;

	//HRESULT hr = cmd.Open(db_connection->GetSession(), query.GetBuffer());
	HRESULT hr = cmd.Open(db_connection->GetSession(), query.GetString());
	if (FAILED(hr))
		return -1;

	while ((hr = cmd.MoveNext()) == S_OK) {
		++row_count;
	}
	return 0;
}

int COLEDB_HANDLER::GetTBLoginExist(int& row_count, CString query)
{
	CCommand<CAccessor<CTB_LOGIN>, CRowset, CMultipleResults> cmd;

	HRESULT hr = cmd.Open(db_connection->GetSession(), query.GetBuffer());
	if (FAILED(hr))
		return -1;

	while ((hr = cmd.MoveNext()) == S_OK) {
		return 1;
	}
	return 0;
}

int COLEDB_HANDLER::InsertTBUser(CString query)
{
	CCommand<CNoAccessor, CNoRowset> cmd;

	HRESULT hr = CoInitialize(0);

	hr = cmd.Open(db_connection->GetSession(), query.GetBuffer());
	cmd.Close();
	if (FAILED(hr))
		return 0;
	return 1;
}

int COLEDB_HANDLER::InsertTBLogin(CString query)
{
	CCommand<CNoAccessor, CNoRowset> cmd;

	HRESULT hr = CoInitialize(0);

	hr = cmd.Open(db_connection->GetSession(), query.GetBuffer());
	cmd.Close();
	if (FAILED(hr))
		return 0;
	return 1;
}

int COLEDB_HANDLER::DeleteTBLogin(CString query)
{
	CCommand<CNoAccessor, CNoRowset> cmd;

	HRESULT hr = CoInitialize(0);

	hr = cmd.Open(db_connection->GetSession(), query.GetBuffer());
	cmd.Close();
	if (FAILED(hr))
		return 0;
	return 1;
}

std::list<QUERY_RESULT> COLEDB_HANDLER::SelectTBUser(CString query)
{
	CCommand<CDynamicAccessor, CRowset, CMultipleResults> cmd;

	WCHAR* a = (WCHAR*)malloc(query.GetLength() * 2);
	int k = query.GetLength();
	a[k] = '\0';
	memcpy(a, query.GetBuffer(), query.GetLength() * 2);
	//HRESULT hr = cmd.Open(db_connection->GetSession(), query.GetBuffer(), NULL, NULL, DBGUID_DBSQL, true);
	HRESULT hr = cmd.Open(db_connection->GetSession(), a, NULL, NULL, DBGUID_DBSQL, true);

	QUERY_RESULT result;

	std::list<QUERY_RESULT> queryResult;

	if (FAILED(hr)) {
		result.type = -1;
		queryResult.push_back(result);
		return queryResult;
	}

	bool checkFirst = true;	// first is true => Column Name

	CString data = _T("");

	RESULT_DATA result_data;
	while ((hr = cmd.MoveNext()) == S_OK) {
		if (checkFirst) {
			// Column Name
			checkFirst = false;
			result.type = 0;
			for (int i = 0; i < cmd.GetColumnCount(); i++) {
				LPOLESTR a = cmd.GetColumnName(i + 1);
				CStringA sss(a);
				data = sss;
				cmd.GetColumnType(i, &result_data.dbType);
				result_data.resultString = data;
				result.result.push_back(result_data);
				result_data.dbType = 0;
				result_data.resultString = _T("");
			}
		}
		else {
			result.type = 1;
			printf("햔재 접속 중 : ");

			for (int i = 0; i < cmd.GetColumnCount(); i++) {
				data = (WCHAR*)cmd.GetValue(i + 1);
				printf("%ls", data);
				result_data.dbType = -1;
				result_data.resultString = data;
				result.result.push_back(result_data);
				result_data.resultString = _T("");
			}
			printf("\n");
		}
		queryResult.push_back(result);
		result.type = 0;
		result.result.clear();
	}
	return queryResult;
}

std::list<QUERY_RESULT> COLEDB_HANDLER::SelectTBLogin(CString query)
{
	// 2023.09.08 문제, SELECT * FROM TB_LOGIN 같은 쿼리는 정상적으로 동작하나, SELECT USER_ID FROM TB_LOGIN 같은 쿼리는 문제가 발생된다. 이유가 멀까.

	CCommand<CDynamicAccessor, CRowset, CMultipleResults> cmd;

	WCHAR* a = (WCHAR*)malloc(query.GetLength() * 2);
	int k = query.GetLength();
	a[k] = '\0';
	memcpy(a, query.GetBuffer(), query.GetLength() * 2);
	//HRESULT hr = cmd.Open(db_connection->GetSession(), query.GetBuffer(), NULL, NULL, DBGUID_DBSQL, true);
	HRESULT hr = cmd.Open(db_connection->GetSession(), a, NULL, NULL, DBGUID_DBSQL, true);
	
	QUERY_RESULT result;

	std::list<QUERY_RESULT> queryResult;

	if (FAILED(hr)) {
		result.type = -1;
		queryResult.push_back(result);
		return queryResult;
	}

	bool checkFirst = true;	// first is true => Column Name

	CString data = _T("");

	RESULT_DATA result_data;
	while ((hr = cmd.MoveNext()) == S_OK) {
		if (checkFirst) {
			// Column Name
			checkFirst = false;
			result.type = 0;
			for (int i = 0; i < cmd.GetColumnCount(); i++) {
				LPOLESTR a = cmd.GetColumnName(i+1);
				CStringA sss(a);
				data = sss;
				cmd.GetColumnType(i, &result_data.dbType);
				result_data.resultString = data;
				result.result.push_back(result_data);
				result_data.dbType = 0;
				result_data.resultString = _T("");
			}
		}
		else {
			result.type = 1;
			printf("햔재 접속 중 : ");

			for (int i = 0; i < cmd.GetColumnCount(); i++) {
				data = (WCHAR*)cmd.GetValue(i + 1);
				printf("%ls",data);
				result_data.dbType = -1;
				result_data.resultString = data;
				result.result.push_back(result_data);
				result_data.resultString = _T("");
			}
			printf("\n");
		}
		queryResult.push_back(result);
		result.type = 0;
		result.result.clear();
	}
	return queryResult;
}