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

// SP ����Ʈ (�߰� �� �� �� ���� �߰�)
enum TABLE_NO {
	TB_LOGIN,
	TB_USER,

	MAX_STORE_NO	// <- �̰� �Ʒ��� SP ���� ����
};