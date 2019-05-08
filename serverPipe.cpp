#include <stdio.h>
#include <Windows.h>//�������÷��� ���
#include <tchar.h>//�����ڵ� ��Ƽ����Ʈ ����ϱ����� ���
#pragma warning(disable: 4996)//scanf����ó��

#define PIPE_NAME	"\\\\.\\pipe\\test_pipe"     
// ������ �̸� ����, �� �������� ����� �� ����

//handle���� �Ķ������ Ŭ���̾�Ʈ�� ��������ִ� �Լ�
int ConnectClient(HANDLE hNamePipe)
{
	TCHAR recvMessage[100];//���� �޼���
	TCHAR sendMessage[100];//���� �޼���
	DWORD recvSize;//���� �޼��� ũ��
	DWORD sendSize;//���� �޼��� ũ��
	TCHAR value[1024]; //ȯ�溯�� ũ��

	TCHAR command[] = _T("Client"); //���� ȯ�溯�� ����

	STARTUPINFO si = { 0, };//process�� ���ڰ� �ʱ�ȭ ����
	PROCESS_INFORMATION pi = { 0, };//process�� ���ڰ� �ʱ�ȭ ����

	SetEnvironmentVariable(command, _T("Connet"));//ȯ�溯�� ��
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
		_tprintf("Server ȯ�溯�� üũ �Ϸ�!\n");

	while (1)
	{
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
		)))          // 
		{
			//����ó��
			_tprintf(_T("WriteFile error! \n"));
			return -1;
		}
		FlushFileBuffers(hNamePipe);
		//recvSize -> NULL ������ ����Ʈ ��
		if (!(ReadFile(
			hNamePipe,//�ڵ鰪
			recvMessage,//�����̸�
			sizeof(recvMessage) - sizeof(TCHAR) * 1,//����ũ��
			&recvSize,
			NULL
		)))
		{
			//����ó��
			printf("ReadFile error! \n");
			return -1;
		}
		recvMessage[recvSize / sizeof(TCHAR) - 1] = _T('\x00');
		_tprintf(_T("Recv Message : %s \n"), recvMessage);
	}
	return 1;
}

int _tmain(int argc, TCHAR* argv[])
{
	HANDLE hNamePipe;//�̸��ִ� ������ ����
	TCHAR pipe_name[] = _T(PIPE_NAME);     // #define �� ������ �̸�
	_tprintf(_T("==== Server ==== \n"));
	//NamedPipe ����
	hNamePipe = CreateNamedPipe(
		pipe_name,//lpName
		PIPE_ACCESS_DUPLEX,//dwOpenMode
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,//dwPipeMode
		PIPE_UNLIMITED_INSTANCES,//nMaxInstances
		0,//nOutBufferSize
		0,//nInBufferSize
		20000,       // ��� Timeout �ð�
		NULL//lpSecurityAttributes
	);
	//����ó��
	if (hNamePipe == INVALID_HANDLE_VALUE)
	{
		printf("CreateNamePipe error! \n");
		return -1;
	}

	while (1)
	{
		//������ Named Pipe�� �ڵ��� ������ �� ������ ���
		if (!(ConnectNamedPipe(hNamePipe, NULL)))
		{
			CloseHandle(hNamePipe);
			return -1;
		}
		else
		{
			if (ConnectClient(hNamePipe) == -1)
				break;
		}
	}
	DisconnectNamedPipe(hNamePipe);//�������
	CloseHandle(hNamePipe);//usageī���� ��ȯ
	_tprintf(_T("Program exit! \n"));
	return 1;
}