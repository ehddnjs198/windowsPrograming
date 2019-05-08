#include<stdio.h>
#include<Windows.h>
#include<tchar.h>
#define SLOT_NAME_R _T("\\\\.\\mailslot\\mailbox_R")
#define SLOT_NAME_S _T("\\\\.\\mailslot\\mailbox_S")
//Rececver Sender의 메일슬롯의 주소값


int _tmain(int argc, TCHAR* args[])
{
	HANDLE hMailSlot_R; //자료형이 HANDLE값인 CreateMailSlot을담을 변수
	HANDLE hMailSlot_S;	//자료형이 HANDLE값인 CreateMailSlot을담을 변수
	TCHAR message_R[50]; //보낼 메세지
	TCHAR message_S[50]; //받을 메세지
	DWORD bytesRead; //ReadFile에 한번에 읽어들일 크기
	DWORD bytesWrite; //RightFile에 한번에 읽어들일 크기

	hMailSlot_R = CreateMailslot(
		SLOT_NAME_R, //자료형이 LPCSTR인 IpName값
		0, //자료형이 DWORD인 maxmessage크기
		MAILSLOT_WAIT_FOREVER, //자료형이 DWORD 기다리는시간
		NULL //보안파트
	);
	//에러처리
	if (hMailSlot_R == INVALID_HANDLE_VALUE)
	{
		_tprintf(_T("Unable to create mailslot! \n"));
		return -1;
	}
	Sleep(5000);//5초의 대기시간

	hMailSlot_S = CreateFile(
		SLOT_NAME_S,//자료형이 LPCSTR인 IpName값
		GENERIC_WRITE, //파일을 열거나 쓰기 등을 할때 엑세스권한 지정 
		FILE_SHARE_READ,//파일의 공유 모드 지정
		NULL,//보안파트
		OPEN_EXISTING,//파일이 해당위치에 존재하는지 여부
		FILE_ATTRIBUTE_NORMAL,//생성된 파일의 속성을 지정
		NULL//GENERIC_READ 엑세스 권한을 가진 템플릿 파일의 유효한 핸들
	);
	//에러처리
	if (hMailSlot_S == INVALID_HANDLE_VALUE)
	{
		_tprintf(_T("Unable to create mailslot! \n"));
		return 1;
	}

	_fputts(_T("==========메일슬롯 sender=========="), stdout);

	while (1)
	{
		_fputts(_T("\n보내는 메세지 = "), stdout);
		_fgetts(message_S, sizeof(message_S) / sizeof(TCHAR), stdin);//문자열입력
		//파일이 안써질때 usage count -1 시키는 if문
		if (!WriteFile(hMailSlot_S, message_S, _tcslen(message_S) * sizeof(TCHAR), &bytesWrite, NULL))
		{
			_fputts(_T("Unable to write!"), stdout);
			CloseHandle(hMailSlot_S);
			return 1;
		}
		//파일이 안읽어질때 usage count -1 시키는 if문
		if (!ReadFile(hMailSlot_R, message_R, sizeof(TCHAR) * 50, &bytesRead, NULL))
		{
			_fputts(_T("Unable to read \n"), stdout);
			CloseHandle(hMailSlot_R);
			return 1;
		}
		//exit 입력하면 종료 하는 if문
		if (!_tcsncmp(message_R, _T("exit"), 4))
		{
			_fputts(_T("Unable to write!"), stdout);
			break;
		}
		//받은 메세지 읽어지는 구간
		_fputts(_T("\n"), stdout);
		message_R[bytesRead / sizeof(TCHAR)] = 0;
		_fputts(_T("받는 메세지 = "), stdout);
		_fputts(message_R, stdout);
		_fputts(_T("\n"), stdout);
	}
	CloseHandle(hMailSlot_R);

}