#include<stdio.h>
#include<Windows.h>
#include<tchar.h>
#define SLOT_NAME_R _T("\\\\.\\mailslot\\mailbox_R")
#define SLOT_NAME_S _T("\\\\.\\mailslot\\mailbox_S")

int _tmain(int argc, TCHAR* args[])
{
	HANDLE hMailSlot_R;//�ڷ����� HANDLE���� CreateMailSlot������ ����
	HANDLE hMailSlot_S;//�ڷ����� HANDLE���� CreateMailSlot������ ����
	TCHAR message_R[50];//���� �޼���
	TCHAR message_S[50];//���� �޼���
	DWORD bytesRead;//ReadFile�� �ѹ��� �о���� ũ��
	DWORD bytesWrite;//RightFile�� �ѹ��� �о���� ũ��

	hMailSlot_S = CreateMailslot(
		SLOT_NAME_S,//�ڷ����� LPCSTR�� IpName��
		0,//�ڷ����� DWORD�� maxmessageũ��
		MAILSLOT_WAIT_FOREVER,//�ڷ����� DWORD ��ٸ��½ð�
		NULL);//������Ʈ
	//����ó��
	if (hMailSlot_S == INVALID_HANDLE_VALUE)
	{
		_tprintf(_T("Unable to create mailslot! \n"));
		return -1;
	}
	Sleep(5000);//5���� ���ð�

	hMailSlot_R = CreateFile(
		SLOT_NAME_R,//�ڷ����� LPCSTR�� IpName��
		GENERIC_WRITE,//������ ���ų� ���� ���� �Ҷ� ���������� ���� 
		FILE_SHARE_READ,//������ ���� ��� ����
		NULL,//������Ʈ
		OPEN_EXISTING,//������ �ش���ġ�� �����ϴ��� ����
		FILE_ATTRIBUTE_NORMAL,//������ ������ �Ӽ��� ����
		NULL//GENERIC_READ ������ ������ ���� ���ø� ������ ��ȿ�� �ڵ�
	);
	//����ó��
	if (hMailSlot_R == INVALID_HANDLE_VALUE)
	{
		_tprintf(_T("Unable to create mailslot! \n"));
		return 1;
	}

	_fputts(_T("=========���Ͻ��� receiver=========="), stdout);

	while (1)
	{
		//������ ���о����� usage count -1 ��Ű�� if��
		if (!ReadFile(hMailSlot_S, message_S, sizeof(TCHAR) * 50, &bytesRead, NULL))
		{
			_fputts(_T("Unable to read \n"), stdout);
			CloseHandle(hMailSlot_S);
			return 1;
		}
		//exit �Է��ϸ� ���� �ϴ� if��
		if (!_tcsncmp(message_S, _T("exit"), 4))
		{
			_fputts(_T("Unable to write!"), stdout);
			break;
		}
		//���� �޼��� �о����� ����
		_fputts(_T("\n"), stdout);
		message_S[bytesRead / sizeof(TCHAR)] = 0;
		_fputts(_T("�޴� �޼��� = "), stdout);
		_fputts(message_S, stdout);
		_fputts(_T("\n"), stdout);

		_fputts(_T("\n������ �޼��� = "), stdout);
		_fgetts(message_R, sizeof(message_R) / sizeof(TCHAR), stdin);
		//������ �Ƚ����� usage count -1 ��Ű�� if��
		if (!WriteFile(hMailSlot_R, message_R, _tcslen(message_R) * sizeof(TCHAR), &bytesWrite, NULL))
		{
			_fputts(_T("Unable to write!"), stdout);
			CloseHandle(hMailSlot_R);
			return 1;
		}
	}
	CloseHandle(hMailSlot_R);

}