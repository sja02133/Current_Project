#pragma once
#include <locale.h>

// 에러 코드에 대한 정의.

// 로그인 시,
// 1000~
#define ERROR_LOGIN_FAILED 1000			// 알수 없는 에러
#define ERROR_LOGIN_EXIST_SESSION 1001	// 로그인 세션이 이미 존재함.
#define ERROR_LOGIN_ERROR_ID 1002		// 로그인 시도 중 ID 혹은 Password 실패
#define ERROR_LOGIN_ERROR_NOT_USE_ID 1003	// 예를 들어 휴먼 처리같이 회원가입은 되어 있으나 사용할 수 없는 ID
// --

