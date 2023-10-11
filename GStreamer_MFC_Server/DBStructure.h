#pragma once

#include "dbDefine.h"

class CBaseProperty
{
public:
	CBaseProperty() {}
	CBaseProperty(const WCHAR* query_string) {
		StringCchCopy(query, MAX_QUERY_LENGTH + 1, query_string);
	}

	virtual int GetResult() { return result; }
	virtual int GetOutResult() { return out_result; }
	virtual WCHAR* GetQuery() { return query; }

protected:
	int result = -1;
	int out_result = -1;
	WCHAR query[MAX_QUERY_LENGTH + 1] = { 0, };
};

class CTB_LOGIN : public CBaseProperty{
public:
	CTB_LOGIN() : CBaseProperty(query) {}
	
	int table_no = TABLE_NO(0);
	WCHAR user_id[MAX_NAME_LENGHT + 1] = { 0 };
	byte user_login_st = 0;
	//time_t user_lastlogin_dt;
	WCHAR user_lastlogin_dt[31 + 1] = { 0 };

	BEGIN_COLUMN_MAP(CTB_LOGIN)
		COLUMN_ENTRY(1, user_id)
		COLUMN_ENTRY(2, user_login_st)
		COLUMN_ENTRY(3, user_lastlogin_dt)
	END_COLUMN_MAP()

	const WCHAR query[MAX_QUERY_LENGTH + 1] = { 0, };
};

class CTB_USER : public CBaseProperty {
	int table_no = TABLE_NO(1);
	WCHAR user_id[MAX_NAME_LENGHT + 1] = { 0 };
	WCHAR user_password[MAX_NAME_LENGHT + 1] = { 0 };
	byte user_use_st = 0;
	//time_t user_created_dt;
	WCHAR user_created_dt[31 + 1] = { 0 };


	BEGIN_COLUMN_MAP(CTB_USER)
		COLUMN_ENTRY(1, user_id)
		COLUMN_ENTRY(2, user_password)
		COLUMN_ENTRY(3, user_use_st)
		COLUMN_ENTRY(4, user_created_dt)
	END_COLUMN_MAP()

	WCHAR query[MAX_QUERY_LENGTH + 1] = { 0, };
};