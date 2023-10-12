// CMembershipDlg.cpp : implementation file
//

#include "pch.h"
#include "GStreamer_MFC_Dlg.h"
#include "CMembershipDlg.h"
#include "afxdialogex.h"


// CMembershipDlg dialog

IMPLEMENT_DYNAMIC(CMembershipDlg, CDialog)

CMembershipDlg::CMembershipDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_MEMBERSHIP, pParent)
	, m_MembershipID(_T(""))
	, m_MembershipPassword(_T(""))
	, m_static(_T(""))
{
	this->iter_c_info = theApp.socket.socket_map.find(theApp.socket.mapKey);
	clnt_con = new CCLIENT_CONTROL;
}

CMembershipDlg::~CMembershipDlg()
{
}

void CMembershipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MEMBERSHIP_ID, m_MembershipID);
	DDX_Text(pDX, IDC_EDIT_MEMBERSHIP_PASSWORD, m_MembershipPassword);
	DDX_Text(pDX, IDC_STATIC_ID, m_static);
}


BEGIN_MESSAGE_MAP(CMembershipDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT_MEMBERSHIP_ID, &CMembershipDlg::OnEnChangeEditMembershipId)
	ON_BN_CLICKED(IDC_BUTTON_MEMBERSHIP_JOIN, &CMembershipDlg::OnBnClickedButtonMembershipJoin)
END_MESSAGE_MAP()


// CMembershipDlg message handlers
bool CMembershipDlg::Check_IdExist()
{
	return true;
}

int CMembershipDlg::Check_IdString()
{
	// ��ȯ ��
	// 1 : ����(��� ����)
	// 2 : ���ĺ��� ����.
	// 3 : ���ڸ� ����.
	// 4 : ���ĺ� or ���ڰ� �ƴ� Ư������ ���� ��.

	UpdateData(FALSE);

	bool checkSuccess = true;


	WCHAR* char_data = 0;
	char_data = (WCHAR*)malloc(m_MembershipID.GetLength()*2);
	memset(char_data, 0, m_MembershipID.GetLength() * 2);
	memcpy(char_data, m_MembershipID.GetBuffer(), m_MembershipID.GetLength() * 2);

	// ���ĺ� ���� �˻�.
	// ���� ���� �˻�. 
	bool checkEnglish = false;
	bool checkNumber = false;
	bool checkSpecialText = false;

	for (int i = 0; i < m_MembershipID.GetLength(); i++) {
		if ((char_data[i] >= 'a' && char_data[i] <= 'z') ||
			(char_data[i] >= 'A' && char_data[i] <= 'Z')
			)
		{
			checkEnglish = true;
			break;
		}
	}

	for (int i = 0; i < m_MembershipID.GetLength(); i++) {
		if (
			(char_data[i] >= '0' && char_data[i] <= '9')
			)
		{
			checkNumber = true;
			break;
		}
	}

	for (int i = 0; i < m_MembershipID.GetLength(); i++) {
		if (
			(char_data[i] >= 0 && char_data[i] <= 47) ||
			(char_data[i] >= 58 && char_data[i] <= 64) ||
			(char_data[i] >= 91 && char_data[i] <= 96) ||
			(char_data[i] >=123 && char_data[i] <= 127)
			)
		{
			checkSpecialText = true;
			break;
		}
	}

	free(char_data);

	// ����
	if (!checkSpecialText && checkEnglish && checkNumber)
		return 1;
	else if (!checkSpecialText && checkEnglish && !checkNumber)
		return 2;
	else if (!checkSpecialText && !checkEnglish && checkNumber)
		return 3;
	else if (checkSpecialText)
		return 4;
	else
		return 0;
}

bool CMembershipDlg::Send_JoinMembership()
{
	return true;
}



int CMembershipDlg::Check_Password()
{
	// ��ȯ ��
	// 1 : ����(��� ����)
	// 2 : ���ĺ��� ����.
	// 3 : ���ڸ� ����.
	// 4 : ���ĺ� or ���ڰ� �ƴ� Ư������ ���� ��.
	// 5 : �ÿ밡�� Ư������(! @ #)�� ������ ���ĺ� �� ���ڰ� ����
	// -1 : 8���� ����..

	// !, @, # �� �ϳ� ���� + ���ĺ� + ���� ���� 8�� �̻�..
	UpdateData(TRUE);

	if (m_MembershipPassword.GetLength() < 8)
		return -1;

	WCHAR* char_data = 0;
	char_data = (WCHAR*)malloc(m_MembershipPassword.GetLength()*2);
	memset(char_data, 0, m_MembershipPassword.GetLength() * 2);
	memcpy(char_data, m_MembershipPassword.GetBuffer(), m_MembershipPassword.GetLength() * 2);

	// ���ĺ� ���� �˻�.
	// ���� ���� �˻�. 
	bool checkEnglish = false;
	bool checkNumber = false;
	bool checkSpecialText = false;
	bool checkCanUseSpecialText = false;

	for (int i = 0; i < m_MembershipPassword.GetLength(); i++) {
		if ((char_data[i] >= 'a' && char_data[i] <= 'z') ||
			(char_data[i] >= 'A' && char_data[i] <= 'Z')
			)
		{
			checkEnglish = true;
			break;
		}
	}

	for (int i = 0; i < m_MembershipPassword.GetLength(); i++) {
		if (
			(char_data[i] >= '0' && char_data[i] <= '9')
			)
		{
			checkNumber = true;
			break;
		}
	}

	// Ư�� ���� ! @ # �˻�.
	int specialTextCount = 0;
	for (int i = 0; i < m_MembershipPassword.GetLength(); i++) {
		if (
			char_data[i] == 33 || char_data[i] == 35 || char_data[i] == 64
			)
		{
			checkCanUseSpecialText = true;
			specialTextCount++;
		}
	}

	for (int i = 0; i < m_MembershipPassword.GetLength(); i++) {
		if (
			(char_data[i] >= 0 && char_data[i] <= 47)||
			(char_data[i] >= 58 && char_data[i] <= 64) ||
			(char_data[i] >= 91 && char_data[i] <= 96) ||
			(char_data[i] >= 123 && char_data[i] <= 127)
			)
		{
			checkSpecialText = true;
			break;
		}
	}

	// ��� ������ Ư�����ڰ� ���� ��,
	if (checkSpecialText && checkCanUseSpecialText) {
		if (checkEnglish && checkNumber && specialTextCount > 1)	// ��� ������ Ư�������� ������ 1���� �ƴ� ���.
			return 6;
		else if (checkEnglish && checkNumber)	// ����
			return 1;
		else if (checkEnglish && !checkNumber)	// ���ڰ� ����
			return 2;
		else if (!checkEnglish && checkNumber)	// ���ĺ��� ����
			return 3;
		else if(!checkEnglish && !checkNumber)	// ��� ������ Ư�����ڸ� ���� ��
			return 5;
	}
	else if (checkSpecialText && !checkCanUseSpecialText) {	// ��� �Ұ����� Ư�����ڸ� ���� ��
		return 4;
	}
	else
		return 0;
}

void CMembershipDlg::OnEnChangeEditMembershipId()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	
	UpdateData(TRUE);

	CString staticFirstText = _T("���� : ");

	if (checkFirst == true || !m_MembershipID.IsEmpty()) {
		checkFirst = false;
		
		int checkText = Check_IdString();
		if (checkText >= 2) {
			if(checkText == 2)
				m_static.SetString(staticFirstText + _T("���ĺ��� ����."));
			else if (checkText == 3)
				m_static.SetString(staticFirstText + _T("���ڸ� ����."));
			else if (checkText == 4)
				m_static.SetString(staticFirstText + _T("Ư������ ����."));
			else
				m_static.SetString(staticFirstText + _T("�˼� ���� ����."));

			UpdateData(FALSE);

			return;
		}
		
		CString data = _T("");
		CString id = _T("id ");

		int length = id.GetLength() + m_MembershipID.GetLength();

		data += id + m_MembershipID;

		char type = 'I';

		
		if (clnt_con->SendData(type, data.GetBuffer(), data.GetLength(), iter_c_info->second)) {
			while (true) {
				auto iter_c_info = theApp.socket.socket_map.find(theApp.socket.mapKey);

				if (iter_c_info->second.last_type == 'R' && iter_c_info->second.last_type_success == 1) {
					// �ߺ� ���̵� ����..
					m_static.SetString(staticFirstText + _T("���̵� ��� ����."));
					checkIdSuccess = true;
					break;
				}
				else if (iter_c_info->second.last_type == 'R' && iter_c_info->second.last_type_success == 0) {
					// �ߺ� ���̵� ����
					m_static.SetString(staticFirstText + _T("���̵� �ߺ���."));
					checkIdSuccess = false;
					break;
				}
				else {
					Sleep(100);
				}
			}
			UpdateData(FALSE);
		}
	}

}


void CMembershipDlg::OnBnClickedButtonMembershipJoin()
{
	// TODO: Add your control notification handler code here
	
	// ���߿� �߰��� �빮�� ���� or �ѱ� üũ ��ɵ� �ִ´�.

	if (!checkIdSuccess)
		MessageBox(_T("���̵� Ȯ�� �ʿ�"));

	int check_password = 0;
	check_password = Check_Password();

	switch (check_password) {
	case 1:
		// ���� -> ȸ�� ���� �ǽ�.
		SendMembershipJoin();
		break;
	case 2:
		// ���ĺ��� ����
		MessageBox(_T("����, Ư������(!,@,#) �� �ϳ��� �����Ͻÿ�."));
		break;
	case 3:
		// ���ڸ� ����
		MessageBox(_T("���ĺ�, Ư������(!,@,#) �� �ϳ��� �����Ͻÿ�."));
		break;
	case 4:
		// ����� �� ���� Ư������.
		MessageBox(_T("��� ������ Ư������(!,@,#) ������ Ȯ���Ͻÿ�."));
		break;
	case 5:
		// ��� ���� Ư�����ڴ� ������ ���� Ȥ�� ���ĺ��� ����.
		MessageBox(_T("���ĺ�, ���� ��θ� �����Ͻÿ�."));
		break;
	case -1:
		// �ڸ��� 8 ����
		MessageBox(_T("��й�ȣ�� 8�� �̻��̾�� ��"));
		break;
	}
}

bool CMembershipDlg::SendMembershipJoin()
{
	UpdateData(TRUE);

	CString data = _T("");
	CString id = _T("id ");
	CString password = _T(" password ");
	int length = id.GetLength() + this->m_MembershipID.GetLength() + password.GetLength() + this->m_MembershipPassword.GetLength();

	data += id + this->m_MembershipID + password + this->m_MembershipPassword;

	char type = 'M';


	
	if (clnt_con->SendData(type, data.GetBuffer(), data.GetLength(), iter_c_info->second)) {
		int count = 0;

		while (true) {
			auto iter_c_info = theApp.socket.socket_map.find(theApp.socket.mapKey);

			if (iter_c_info->second.last_type == 'R' && iter_c_info->second.last_type_success == 1) {
				// ����
				int status = 0;
				status = MessageBox(_T("ȸ������ ����! �ٽ� �α��� �Ͻÿ�."));
				if (status == IDOK || status == IDCANCEL) {
					//this->OnClose();
					::SendMessage(this->m_hWnd, WM_CLOSE, NULL, NULL);
				}
				break;
			}
			else if (iter_c_info->second.last_type == 'R' && iter_c_info->second.last_type_success == 0) {
				// ����..
				MessageBox(_T("ȸ������ ����"));
				break;
			}
			else {
				count++;
				if (count == 10) {
					MessageBox(_T("���� �ð� �ʰ�, ��õ� �� ��"));
					break;
				}
				Sleep(100);
			}
		}
	}
	return true;
}