
// GStreamer_MFC_Dlg.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "GStreamer_MFC_Dlg.h"
#include "GStreamer_MFC_DlgDlg.h"

#include "../Common/GStreamer_MFC/_TCP_/tcpSocket.h"
#include "../Common/GStreamer_MFC/byteControl/byteControl.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGStreamerMFCDlgApp

BEGIN_MESSAGE_MAP(CGStreamerMFCDlgApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CGStreamerMFCDlgApp construction

CGStreamerMFCDlgApp::CGStreamerMFCDlgApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	
	// 2023.12.07 수정사항.
	// 기존 구조는 클라이언트가 실행되자마자 연결을 시도했었다.
	// -> 이 구조를 로그인 버튼(로그인 시도 시) 클릭 시 연결하는 것으로 변경하도록 한다.
	//this->socket.Initialize(0);
}

CGStreamerMFCDlgApp::~CGStreamerMFCDlgApp()
{
	_CrtDumpMemoryLeaks();
}

// The one and only CGStreamerMFCDlgApp object

CGStreamerMFCDlgApp theApp;


// CGStreamerMFCDlgApp initialization

BOOL CGStreamerMFCDlgApp::InitInstance()
{
// TODO: call AfxInitRichEdit2() to initialize richedit2 library.\n"	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CGStreamerMFCDlgDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CGStreamerMFCDlgApp::MessageBox_ErrorCode(HWND hwnd, int code, SOCKET_INFO& c_info,int messageBoxType)
{
	//c_info.pTCP_SOCKET->GetServerErrorMsg(errorMsg,c_info);

	int pos = 0;

	int totalLength = c_info.sendDataLength;

	WCHAR* tempChar = (WCHAR*)malloc(totalLength*2);
	memcpy(tempChar, c_info.sendData, totalLength*2);

	pos++;	//pass to type 1 byte

	int length = get_byteToInt((char*)&tempChar[pos]);
	pos += sizeof(int);

	int errorCode = get_byteToInt((char*)&tempChar[pos]);
	pos += sizeof(int);

	WCHAR error[1024] = { 0, };
	memcpy(error, &c_info.sendData[pos], (totalLength - pos)*2);

	free(tempChar);
	//CString errorMsg = CString(error);

	// Assumes std::string is encoded in the current Windows ANSI codepage
	//int bufferlen = ::MultiByteToWideChar(CP_ACP, 0, errorMsg.GetBuffer(), errorMsg.GetLength(), NULL, 0);

	//if (bufferlen == 0)
	//{
		// Something went wrong. Perhaps, check GetLastError() and log.
	//	return;
	//}

	// Allocate new LPWSTR - must deallocate it later
	//LPWSTR widestr = new WCHAR[bufferlen + 1];

	//::MultiByteToWideChar(CP_ACP, 0, errorMsg.c_str(), errorMsg.size(), widestr, bufferlen);

	// Ensure wide string is null terminated
	//widestr[bufferlen] = 0;

	// Do something with widestr

	//MessageBox(hwnd,widestr, _T("오류"), messageBoxType);

	AfxMessageBox(error, MB_OK);

//	MessageBox(hwnd,errorMsg.GetBuffer(), _T("오류"), messageBoxType);
	//delete[] widestr;
}