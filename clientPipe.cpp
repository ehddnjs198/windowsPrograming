#include <stdio.h>
#include <Windows.h>//�������÷��� ���
#include <tchar.h>//�����ڵ� ��Ƽ����Ʈ ����ϱ����� ���
#pragma warning(disable: 4996)//scanfó��

#define PIPE_NAME	"\\\\.\\pipe\\test_pipe"     
// ������ ������ �̸��� ������ ������ ���

//handle���� �Ķ������ Ŭ���̾�Ʈ�� ��������ִ� �Լ�
int ConnectServer(HANDLE hNamePipe)
{
	TCHAR recvMessage[100];//���� �޼���
	TCHAR sendMessage[100];//���� �޼���
	DWORD recvSize;//���� �޼��� ũ��
	DWORD sendSize;//���� �޼��� ũ��
	TCHAR value[1024];//ȯ�溯�� ũ��

	TCHAR command[] = _T("Server");//Ŭ���̾�Ʈ ȯ�溯�� ����

	STARTUPINFO si = { 0, };//process�� ���ڰ� �ʱ�ȭ ����
	PROCESS_INFORMATION pi = { 0, };//process�� ���ڰ� �ʱ�ȭ ����

	SetEnvironmentVariable(command, _T("Connect"));//ȯ�溯�� ��
	CreateProcess(
		NULL,//lpApplicationName
		command,//lpCommandLine
		NULL,//lpProcessAttributes
		NULL,//lpThreadAttributes
		FALSE,//bInheritHandles
		CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,//dwCreationFlags
		NULL,//lpEnvironment
		NULL,// lpCurrentDirectory,
		&si,//lpStartupInfo
		&pi//lpProcessInformation
	);

	if (SetEnvironmentVariable(command, value) > 0)//ȯ�溯�� ���� Ȯ��
		_tprintf("Client ȯ�溯�� üũ�Ϸ�\n");

	while (1)
	{
		if (!(ReadFile(
			hNamePipe,//�ڵ鰪
			recvMessage,//�����̸�
			sizeof(recvMessage) - sizeof(TCHAR) * 1,//���ۻ�����
			&recvSize,
			NULL
		)))
		{
			_tprintf(_T("ReadFile error! \n"));
			return -1;
		}

		recvMessage[recvSize / sizeof(TCHAR) - 1] = _T('\x00');
		_tprintf(_T("Recv Message : %s \n"), recvMessage);
		_tprintf(_T("Input Send Message : "));
		_tscanf(_T("%s"), sendMessage);
		//sendSize -> NULL ������ ����Ʈ ��
		//ReadFile�� ���ؼ� Ŭ���̾�Ʈ ���� �о���̱�
		if (!(WriteFile(
			hNamePipe,//�ڵ鰪
			sendMessage,//�����̸�
			(_tcslen(sendMessage) + 1) * sizeof(TCHAR),//����ũ��
			&sendSize,
			NULL
		)))
		{
			_tprintf(_T("WriteFile error! \n"));
			return -1;
		}
		FlushFileBuffers(hNamePipe);
	}
	return 1;
}



int _tmain(int argc, TCHAR* argv[])
{
	TCHAR pipe_name[] = _T(PIPE_NAME);
	HANDLE hNamePipe;//�̸��ִ� ������ ����

	_tprintf(_T("==== Client ==== \n"));
	while (1)
	{
		// ������ �̸����� �ڵ� ����
		hNamePipe = CreateFile(pipe_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hNamePipe == INVALID_HANDLE_VALUE)
		{
			_tprintf(_T("CreateFile error! \n"));
			return -1;
		}

		DWORD pipeMode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
		if (!(SetNamedPipeHandleState(hNamePipe, &pipeMode, NULL, NULL)))
		{
			_tprintf(_T("SetNamedPipeHandleState error! \n"));
			CloseHandle(hNamePipe);
			return -1;
		}
		else
		{
			if (ConnectServer(hNamePipe) == -1)
				break;
		}
	}
	CloseHandle(hNamePipe);//usangeī���� ��ȯ
	return 0;
}

