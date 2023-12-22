
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
	clnt_con = new CCLIENT_CONTROL;
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

// ��ũ ������ ���⿡ �̰��� �����Լ��� �ӽ� �����ؼ� �����. 
void Get_Network_Error(int errorCode, CString* str) {
	switch (errorCode) {
	case -1:
		*str = _T("WSAStartUp Failed...");
		break;
	case -2:
		*str = _T("Create Socket Failed...");
		break;
	case -3:
		*str = _T("Bind Failed...");
		break;
	case -4:
		*str = _T("(SERVER) Listen Failed...");
		break;
	case -5:
		*str = _T("(SERVER) Socket is INVALID_SOCKET");
		break;
	case -6:
		*str = _T("(CLIENT) Connect Failed...");
		break;
	default:
		*str = _T("Something happend error in Network connect..");
		break;
	}
}

void CGStreamerMFCDlgDlg::OnBnClickedButtonLogin()
{
	// 2023.12.07
	// ���� ������ ������ �õ��Ѵ�.
	int check_connect = theApp.socket.Initialize(0);

	if (check_connect != 1) {
		// ��Ʈ��ũ ���� �� ���� �߻�..
		CString errorMsg = _T("");
		Get_Network_Error(check_connect,&errorMsg);
		AfxMessageBox(errorMsg, MB_OK);
		return;
	}

	UpdateData(TRUE);	// ���� �����´�.

	// check tcp socket success
	CString data = _T("");
	CString id = _T("id ");
	CString password = _T(" password ");

	if (this->m_loginID.GetLength() == 0) {
		AfxMessageBox(_T("���̵� �Է����ּ���."),MB_OK);
		return;
	}
	
	if (this->m_loginPassword.GetLength() == 0) {
		AfxMessageBox(_T("��й�ȣ�� �Է����ּ���."), MB_OK);
		return;
	}

	// 2023.12.05
	// ��й�ȣ�� �״�� ������ �ʰ�, md5 Ȥ�� sha-256 �������� ��ȣȭ�Ͽ� ������ �Ѵ�.
	BYTE buf[SHA256_BLOCK_SIZE] = { 0, };
	SHA256_CTX ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, (BYTE*)this->m_loginPassword.GetBuffer(), this->m_loginPassword.GetLength());
	sha256_final(&ctx, buf);

	int length = id.GetLength() + this->m_loginID.GetLength() + password.GetLength() + this->m_loginPassword.GetLength();
	
	// 2023.12.06 ���� �ؽð����� ��й�ȣ ������ �� �̰� �ּ� Ǯ�����.
	//int length = sizeof(WCHAR) + (sizeof(int)*2) + ((id.GetLength() + this->m_loginID.GetLength() + password.GetLength())*2) + (SHA256_BLOCK_SIZE);

	//int pos = 0;
	//char* set_loginData = (char*)malloc(length);
	////pos += sizeof(WCHAR) + sizeof(int);		// type + total length
	//memset(set_loginData, 0, length);
	//
	//WCHAR type = 'L';
	//memcpy(&set_loginData[pos], &type, sizeof(WCHAR));
	//pos += sizeof(WCHAR);
	//pos += sizeof(int)*2;
	//memcpy(&set_loginData[pos], id.GetBuffer(), id.GetLength() * 2);
	//pos += id.GetLength()*2;
	//memcpy(&set_loginData[pos], this->m_loginID.GetBuffer(), this->m_loginID.GetLength() * 2);
	//pos += this->m_loginID.GetLength() * 2;
	//memcpy(&set_loginData[pos], password.GetBuffer(), password.GetLength() * 2);
	//pos += password.GetLength() * 2;
	//memcpy(&set_loginData[pos], buf, SHA256_BLOCK_SIZE);
	//pos += SHA256_BLOCK_SIZE;
	//memcpy(&set_loginData[2], &pos, sizeof(int));

	data += id + this->m_loginID + password + this->m_loginPassword;

	// �α��� �� ������ �������丮�� ����� �Ѵ�.
	CRegKey RegKey;
	LONG result = RegKey.Create(HKEY_CURRENT_USER, _T("SOFTWARE\\GStreamer_MFC"));

	if (result == ERROR_SUCCESS) {
		result = RegKey.SetStringValue(_T("ID"), this->m_loginID, REG_EXPAND_SZ);
		if(result == ERROR_SUCCESS)
			RegKey.Close();
	}

	WCHAR type = 'L';

	auto iter_c_info = theApp.socket.socket_map.find(theApp.socket.mapKey);
	if (clnt_con->SendData(type,data.GetBuffer(),data.GetLength(), iter_c_info->second)) {
	//if (clnt_con->JustSendData(set_loginData, pos, iter_c_info->second.pTCP_SOCKET, iter_c_info->second.cSock)) {
		int count = 0;
		//free(set_loginData);
		while (true) {
			iter_c_info = theApp.socket.socket_map.find(theApp.socket.mapKey);

			//if (iter_c_info->second.last_type == 'R' && iter_c_info->second.last_type_success == 1) {
			//	// ����
			//	memcpy(iter_c_info->second.ID, this->m_loginID.GetBuffer(), this->m_loginID.GetLength() * 2);

			//	CChattingRoom dlg;
			//	dlg.DoModal();
			//	break;
			//}
			//else if (iter_c_info->second.last_type == 'R' && iter_c_info->second.last_type_success == 0) {
			//	// ����..
			//	MessageBox(L"�α��� ����");
			//	break;
			//}
			//else if (iter_c_info->second.last_type == 'E' && iter_c_info->second.last_type_success == 0) {
			//	// ����..
			//	theApp.MessageBox_ErrorCode(this->GetSafeHwnd(), iter_c_info->second.errorCode, iter_c_info->second, 0);
			//	break;
			//}

			if (iter_c_info->second.last_type == 'R' && iter_c_info->second.last_type_success == 1) {
				// ����
				memcpy(iter_c_info->second.ID, this->m_loginID.GetBuffer(), this->m_loginID.GetLength() * 2);
				//memcpy(theApp.socket.clientInfo->ID, this->m_loginID.GetBuffer(), this->m_loginID.GetLength() * 2);

				CChattingRoom dlg;
				dlg.DoModal();
				break;
			}
			else if (iter_c_info->second.last_type == 'R' && iter_c_info->second.last_type_success == 0) {
				// ����..
				MessageBox(L"�α��� ����");
				break;
			}
			else if (iter_c_info->second.last_type == 'E' && iter_c_info->second.last_type_success == 0) {
				// ����..
				theApp.MessageBox_ErrorCode(this->GetSafeHwnd(), theApp.socket.clientInfo->errorCode,
					iter_c_info->second, 0);
				break;
			}
			else {
				count++;
				if (count == 10) {
					// 2023.09.01 ������ �𸣰����� MessageBox�� �����ϸ� ������ �����. ������?
					// 
					//MessageBox(L"���� �ð� �ʰ�");
					break;
				}
				Sleep(1000);
			}
		}
	}
	else {
		//free(set_loginData);
	}
}


void CGStreamerMFCDlgDlg::OnBnClickedButtonJoinMembership()
{
	// TODO: Add your control notification handler code here
	CMembershipDlg dlg;
	dlg.DoModal();
}
