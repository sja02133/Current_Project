
// GStreamer_MFC_Dlg.h : main header file for the PROJECT_NAME application
//

#pragma once

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "../Common/GStreamer_MFC/_TCP_/tcpSocket.h"
#include "../Common/GStreamer_MFC/ERROR_CODE/errorCode.h"
#include "../Common/GStreamer_MFC/byteControl/byteControl.h"

// CGStreamerMFCDlgApp:
// See GStreamer_MFC_Dlg.cpp for the implementation of this class
//



class CGStreamerMFCDlgApp : public CWinApp
{
public:
	CGStreamerMFCDlgApp();
	~CGStreamerMFCDlgApp();
// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
public:
	CTCP_SOCKET socket;
	void MessageBox_ErrorCode(HWND hwnd, int code, SOCKET_INFO& c_info, int messageBoxType);
};

extern CGStreamerMFCDlgApp theApp;
