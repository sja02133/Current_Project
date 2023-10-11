#pragma once

#include <Windows.h>
#include <oledb.h>
#include <stdio.h>
#include <atldbcli.h>
#include <strsafe.h>
#include <memory>
#include <string>

#include <atlstr.h>

constexpr auto MAX_DB_STRING_LENGTH = 2048;
constexpr auto MAX_QUERY_LENGTH = 2048;

constexpr auto MAX_NAME_LENGHT = 10;

// SP 리스트 (추가 될 시 이 곳에 추가)
enum TABLE_NO {
	TB_LOGIN,
	TB_USER,

	MAX_STORE_NO	// <- 이곳 아래에 SP 정의 금지
};