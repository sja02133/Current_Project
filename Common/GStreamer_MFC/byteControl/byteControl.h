#pragma once

#include <stdint.h>
#include <string.h>
#include <string>

#include <Windows.h>

//#define ULONG unsigned long long
//#define UINT unsigned int
//#define USHORT unsigned short
//
//
//#define BOOL bool
//#define TRUE true
//#define FALSE false
//
//#define u_char unsigned char

//ULONG CharSet(const char* str, int bytelen);
//void UTF8ToANSI(const char* pszCode, CString* str, int* existing_pos);

BOOL is_network_order(unsigned char* a);
BOOL is_network_order(char* a);
unsigned short get_byteToShort(unsigned char* byte);

unsigned int get_byteToInt(unsigned char* byte);
unsigned int get_byteToInt(char* byte);

char* ANSIToUTF8(char* pszCode, int& utf8Len);
char* UTF8toANSI(char* pszCode,int len);


template <class T>
void endswap(T* objp)
{
    unsigned char* memp = reinterpret_cast<unsigned char*>(objp);
    std::reverse(memp, memp + sizeof(T));
}

template <class T>
T* endswap_t(T* objp)
{
    unsigned char* memp = reinterpret_cast<unsigned char*>(objp);
    std::reverse(memp, memp + sizeof(T));
    return objp;
}