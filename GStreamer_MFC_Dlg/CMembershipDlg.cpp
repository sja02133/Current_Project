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
	// 반환 값
	// 1 : 정상(사용 가능)
	// 2 : 알파벳만 존재.
	// 3 : 숫자만 존재.
	// 4 : 알파벳 or 숫자가 아닌 특수문자 등이 들어감.

	UpdateData(FALSE);

	bool checkSuccess = true;


	WCHAR* char_data = 0;
	char_data = (WCHAR*)malloc(m_MembershipID.GetLength()*2);
	memset(char_data, 0, m_MembershipID.GetLength() * 2);
	memcpy(char_data, m_MembershipID.GetBuffer(), m_MembershipID.GetLength() * 2);

	// 알파벳 유무 검사.
	// 숫자 유무 검사. 
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

	// 정상
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
	// 반환 값
	// 1 : 정상(사용 가능)
	// 2 : 알파벳만 존재.
	// 3 : 숫자만 존재.
	// 4 : 알파벳 or 숫자가 아닌 특수문자 등이 들어감.
	// 5 : 시용가능 특수문자(! @ #)가 있지만 알파벳 및 숫자가 없음
	// -1 : 8글자 이하..

	// !, @, # 중 하나 포함 + 알파벳 + 숫자 조합 8자 이상..
	UpdateData(TRUE);

	if (m_MembershipPassword.GetLength() < 8)
		return -1;

	WCHAR* char_data = 0;
	char_data = (WCHAR*)malloc(m_MembershipPassword.GetLength()*2);
	memset(char_data, 0, m_MembershipPassword.GetLength() * 2);
	memcpy(char_data, m_MembershipPassword.GetBuffer(), m_MembershipPassword.GetLength() * 2);

	// 알파벳 유무 검사.
	// 숫자 유무 검사. 
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

	// 특수 문자 ! @ # 검사.
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

	// 사용 가능한 특수문자가 있을 때,
	if (checkSpecialText && checkCanUseSpecialText) {
		if (checkEnglish && checkNumber && specialTextCount > 1)	// 사용 가능한 특수문자의 갯수가 1개가 아닐 경우.
			return 6;
		else if (checkEnglish && checkNumber)	// 정상
			return 1;
		else if (checkEnglish && !checkNumber)	// 숫자가 빠짐
			return 2;
		else if (!checkEnglish && checkNumber)	// 알파벳이 빠짐
			return 3;
		else if(!checkEnglish && !checkNumber)	// 사용 가능한 특수문자만 있을 때
			return 5;
	}
	else if (checkSpecialText && !checkCanUseSpecialText) {	// 사용 불가능한 특수문자만 있을 때
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

	CString staticFirstText = _T("상태 : ");

	if (checkFirst == true || !m_MembershipID.IsEmpty()) {
		checkFirst = false;
		
		int checkText = Check_IdString();
		if (checkText >= 2) {
			if(checkText == 2)
				m_static.SetString(staticFirstText + _T("알파벳만 존재."));
			else if (checkText == 3)
				m_static.SetString(staticFirstText + _T("숫자만 존재."));
			else if (checkText == 4)
				m_static.SetString(staticFirstText + _T("특수문자 존재."));
			else
				m_static.SetString(staticFirstText + _T("알수 없음 오류."));

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
					// 중복 아이디 없음..
					m_static.SetString(staticFirstText + _T("아이디 사용 가능."));
					checkIdSuccess = true;
					break;
				}
				else if (iter_c_info->second.last_type == 'R' && iter_c_info->second.last_type_success == 0) {
					// 중복 아이디 존재
					m_static.SetString(staticFirstText + _T("아이디 중복됨."));
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
	
	// 나중에 추가로 대문자 구분 or 한글 체크 기능도 넣는다.

	if (!checkIdSuccess)
		MessageBox(_T("아이디 확인 필요"));

	int check_password = 0;
	check_password = Check_Password();

	switch (check_password) {
	case 1:
		// 정상 -> 회원 가입 실시.
		SendMembershipJoin();
		break;
	case 2:
		// 알파벳만 존재
		MessageBox(_T("숫자, 특수문자(!,@,#) 중 하나를 포함하시오."));
		break;
	case 3:
		// 숫자만 존재
		MessageBox(_T("알파벳, 특수문자(!,@,#) 중 하나를 포함하시오."));
		break;
	case 4:
		// 사용할 수 없음 특수문자.
		MessageBox(_T("사용 가능한 특수문자(!,@,#) 인지를 확인하시오."));
		break;
	case 5:
		// 사용 가능 특수문자는 있지만 숫자 혹은 알파벳이 없음.
		MessageBox(_T("알파벳, 숫자 모두를 포함하시오."));
		break;
	case -1:
		// 자릿수 8 이하
		MessageBox(_T("비밀번호는 8자 이상이어야 함"));
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
				// 성공
				int status = 0;
				status = MessageBox(_T("회원가입 성공! 다시 로그인 하시오."));
				if (status == IDOK || status == IDCANCEL) {
					//this->OnClose();
					::SendMessage(this->m_hWnd, WM_CLOSE, NULL, NULL);
				}
				break;
			}
			else if (iter_c_info->second.last_type == 'R' && iter_c_info->second.last_type_success == 0) {
				// 실패..
				MessageBox(_T("회원가입 실패"));
				break;
			}
			else {
				count++;
				if (count == 10) {
					MessageBox(_T("응답 시간 초과, 재시도 할 것"));
					break;
				}
				Sleep(100);
			}
		}
	}
	return true;
}