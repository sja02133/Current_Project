#pragma once

#include "dbDefine.h"

#include <list>

class COLEDB {
public:
	HRESULT hr;
	CLSID cls_id = CLSID_NULL;
	IDBInitialize* pDbInit;

	CDataSource ds;
	CSession session;
	//CCommand <CAccessor<state>> cust;
	CDBPropSet dbinit;
public:
	COLEDB();
	
	bool InitCDBPropSet();
	bool OpenCDataSource();
	bool StartSession();
	bool Connect();
	CSession GetSession();
	int ReadDB(CString strSQL, int table_no);
	int WriteDB(CString strSQL);
};

typedef struct result
{
	DBTYPE dbType = 0;
	CString resultString;
}RESULT_DATA;

typedef struct query_result {
	int type = 0;	// 0 => Column Name, 1 => Column Value, -1 => error
	std::list<RESULT_DATA> result;
}QUERY_RESULT;


class COLEDB_HANDLER{
public:
	COLEDB_HANDLER();
	~COLEDB_HANDLER();
	typedef int(COLEDB_HANDLER::* DB_FUNC)(int&, CString);
	static COLEDB_HANDLER& GetInstance();
	int DBProcess(TABLE_NO tb_no, int& row_count, CString query);
	void Initialize();
private:
	
	std::unique_ptr<COLEDB> db_connection;
	DB_FUNC func_list_[MAX_STORE_NO];
	
public:
	//SP ÇÔ¼ö
	// Select
	int GetTBLogin(int& row_count , CString query);
	int GetTBUser(int& row_count, CString query);
	int GetTBUserExist(int& row_count, CString query);
	int GetTBLoginExist(int& row_count, CString query);
public:
	// Insert
	int InsertTBUser(CString query);
	int InsertTBLogin(CString query);
public:
	// Delete
	int DeleteTBLogin(CString query);
public:
	// Dynamic Binding
	//std::list<QUERY_RESULT> queryResult;
	std::list<QUERY_RESULT> SelectTBUser(CString query);
	std::list<QUERY_RESULT> SelectTBLogin(CString query);
};
