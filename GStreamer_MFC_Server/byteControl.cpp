#include "byteControl.h"

BOOL is_network_order(unsigned char* a) {
    if (a == 0)
        return FALSE;
    unsigned char p = *a;
    return (p == 0);
}

BOOL is_network_order(char* a) {
    if (a == 0)
        return FALSE;
    unsigned char p = *a;
    return (p == 0);
}

unsigned short get_byteToShort(unsigned char* byte)
{
    USHORT data = 0;
    memcpy(&data, &byte[0], sizeof(USHORT));
    if (byte == 0)
        return 0;
    if (is_network_order(byte)) {
        endswap(&data);
        return data;
    }
    else {
        //host order
        return data;
    }
}

unsigned int get_byteToInt(unsigned char* byte)
{
    int data = 0;
    memcpy(&data, &byte[0], sizeof(UINT));
    if (byte == 0)
        return 0;
    if (is_network_order(byte)) {
        //network order
//        data = ntohl(*reinterpret_cast<int*>(byte));
        //return *(unsigned int*)byte;
        endswap(&data);
        return data;
    }
    else {
        //host order
        return data;
    }
}

unsigned int get_byteToInt(char* byte)
{
    int data = 0;
    memcpy(&data, &byte[0], sizeof(UINT));
    if (byte == 0)
        return 0;
    if (is_network_order(byte)) {
        //network order
//        data = ntohl(*reinterpret_cast<int*>(byte));
        //return *(unsigned int*)byte;
        endswap(&data);
        return data;
    }
    else {
        //host order
        return data;
    }
}

char* ANSIToUTF8(char* pszCode,int& utf8Len)
{
    BSTR bstrCode;
    char* pszUTFCode = NULL;
    int  nLength, nLength2;

    nLength = MultiByteToWideChar(CP_ACP, 0, pszCode, strlen(pszCode), NULL, NULL);
    bstrCode = SysAllocStringLen(NULL, nLength);
    MultiByteToWideChar(CP_ACP, 0, pszCode, strlen(pszCode), bstrCode, nLength);

    nLength2 = WideCharToMultiByte(CP_UTF8, 0, bstrCode, -1, pszUTFCode, 0, NULL, NULL);
    pszUTFCode = (char*)malloc(nLength2 + 1);
    WideCharToMultiByte(CP_UTF8, 0, bstrCode, -1, pszUTFCode, nLength2, NULL, NULL);

    utf8Len = nLength2;

    return pszUTFCode;
}

char* UTF8toANSI(char* pszCode,int len)
{
    BSTR    bstrWide;
    char* pszAnsi = NULL;
    int     nLength;

    nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, NULL, NULL);
    bstrWide = SysAllocStringLen(NULL, nLength);
    MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, bstrWide, nLength);

    nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
    pszAnsi = new char[len + 1];

    WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, len + 1, NULL, NULL);
    SysFreeString(bstrWide);

    return pszAnsi;
}