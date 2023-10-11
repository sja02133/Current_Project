
// GStreamer_MFC_DlgDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "GStreamer_MFC_Dlg.h"
#include "GStreamer_MFC_DlgDlg.h"
#include "afxdialogex.h"

#include "CChattingRoom.h"
#include "CMembershipDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CGStreamerMFCDlgDlg dialog



CGStreamerMFCDlgDlg::CGStreamerMFCDlgDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_LOGIN, pParent)
	, m_loginID(_T(""))
	, m_loginPassword(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGStreamerMFCDlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LOGIN_ID, m_loginID);
	DDX_Text(pDX, IDC_EDIT_LOGIN_PASSWORD, m_loginPassword);
}

BEGIN_MESSAGE_MAP(CGStreamerMFCDlgDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CGStreamerMFCDlgDlg::OnBnClickedButtonLogin)
	ON_BN_CLICKED(IDC_BUTTON_JOIN_MEMBERSHIP, &CGStreamerMFCDlgDlg::OnBnClickedButtonJoinMembership)
END_MESSAGE_MAP()


// CGStreamerMFCDlgDlg message handlers

BOOL CGStreamerMFCDlgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	DWORD dwType = REG_SZ;
	DWORD dwSize = 128;
	HKEY hKey;
	WCHAR szBuffer[128] = { '\0' , };

	LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\GStreamer_MFC"), 0, KEY_READ, &hKey);
	if (ERROR_SUCCESS == lResult)
	{
		RegQueryValueEx(hKey, _T("ID"), NULL, &dwType, (LPBYTE)szBuffer, &dwSize);
	}
	
	if (_tcsclen(szBuffer) != 0) {
		this->m_loginID = szBuffer;
		UpdateData(FALSE);
	}


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGStreamerMFCDlgDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGStreamerMFCDlgDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGStreamerMFCDlgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CGStreamerMFCDlgDlg::OnBnClickedButtonLogin()
{
	UpdateData(TRUE);	// 값을 가져온다.

	// check tcp socket success
	CString data = _T("");
	CString id = _T("id ");
	CString password = _T(" password ");

	if (this->m_loginID.GetLength() == 0) {
		AfxMessageBox(_T("아이디를 입력해주세요."),MB_OK);
		return;
	}
	
	if (this->m_loginPassword.GetLength() == 0) {
		AfxMessageBox(_T("비밀번호를 입력해주세요."), MB_OK);
		return;
	}

	int length = id.GetLength() + this->m_loginID.GetLength() + password.GetLength() + this->m_loginPassword.GetLength();

	data += id + this->m_loginID + password + this->m_loginPassword;

	// 로그인 시 무조건 레지스토리에 기록을 한다.
	CRegKey RegKey;
	LONG result = RegKey.Create(HKEY_CURRENT_USER, _T("SOFTWARE\\GStreamer_MFC"));

	if (result == ERROR_SUCCESS) {
		result = RegKey.SetStringValue(_T("ID"), this->m_loginID, REG_EXPAND_SZ);
		if(result == ERROR_SUCCESS)
			RegKey.Close();
	}

	WCHAR type = 'L';

	//char&& a = 10;

	if (theApp.socket.SendData(type, data.GetBuffer(), data.GetLength(), &theApp.socket)) {
		int count = 0;

		while (true) {
			//auto iter_c_info = theApp.socket.client_map.find(theApp.socket.mapKey);

			//if (iter_c_info->second.last_type == 'R' && iter_c_info->second.last_type_success == 1) {
			//	// 성공
			//	memcpy(iter_c_info->second.ID, this->m_loginID.GetBuffer(), this->m_loginID.GetLength() * 2);

			//	CChattingRoom dlg;
			//	dlg.DoModal();
			//	break;
			//}
			//else if (iter_c_info->second.last_type == 'R' && iter_c_info->second.last_type_success == 0) {
			//	// 실패..
			//	MessageBox(L"로그인 실패");
			//	break;
			//}
			//else if (iter_c_info->second.last_type == 'E' && iter_c_info->second.last_type_success == 0) {
			//	// 실패..
			//	theApp.MessageBox_ErrorCode(this->GetSafeHwnd(), iter_c_info->second.errorCode, iter_c_info->second, 0);
			//	break;
			//}

			if (theApp.socket.clientInfo->last_type == 'R' && theApp.socket.clientInfo->last_type_success == 1) {
				// 성공
				memcpy(theApp.socket.clientInfo->ID, this->m_loginID.GetBuffer(), this->m_loginID.GetLength()*2);
				
				CChattingRoom dlg;
				dlg.DoModal();
				break;
			}
			else if(theApp.socket.clientInfo->last_type == 'R' && theApp.socket.clientInfo->last_type_success == 0) {
				// 실패..
				MessageBox(L"로그인 실패");
				break;
			}
			else if (theApp.socket.clientInfo->last_type == 'E' && theApp.socket.clientInfo->last_type_success == 0) {
				// 실패..
				theApp.MessageBox_ErrorCode(this->GetSafeHwnd(), theApp.socket.clientInfo->errorCode,
					*theApp.socket.clientInfo, 0);
				break;
			}
			else {
				count++;
				if (count == 10) {
					// 2023.09.01 왜인진 모르겠지만 MessageBox를 실행하면 에러가 생긴다. 이유는?
					// 
					//MessageBox(L"응답 시간 초과");
					break;
				}
				Sleep(1000);
			}
		}
	}
}


void CGStreamerMFCDlgDlg::OnBnClickedButtonJoinMembership()
{
	// TODO: Add your control notification handler code here
	CMembershipDlg dlg;
	dlg.DoModal();
}
