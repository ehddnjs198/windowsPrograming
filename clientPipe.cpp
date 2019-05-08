#include <stdio.h>
#include <Windows.h>//윈도우플랫폼 헤더
#include <tchar.h>//유니코드 멀티바이트 사용하기위한 헤더
#pragma warning(disable: 4996)//scanf처리

#define PIPE_NAME	"\\\\.\\pipe\\test_pipe"     
// 서버의 파이프 이름과 동일한 파이프 사용

//handle값이 파라미터인 클라이언트와 연결시켜주는 함수
int ConnectServer(HANDLE hNamePipe)
{
	TCHAR recvMessage[100];//받을 메세지
	TCHAR sendMessage[100];//보낼 메세지
	DWORD recvSize;//받을 메세지 크기
	DWORD sendSize;//보낼 메세지 크기
	TCHAR value[1024];//환경변수 크기

	TCHAR command[] = _T("Server");//클라이언트 환경변수 설정

	STARTUPINFO si = { 0, };//process의 인자값 초기화 변수
	PROCESS_INFORMATION pi = { 0, };//process의 인자값 초기화 변수

	SetEnvironmentVariable(command, _T("Connect"));//환경변수 셋
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

	if (SetEnvironmentVariable(command, value) > 0)//환경변수 설정 확인
		_tprintf("Client 환경변수 체크완료\n");

	while (1)
	{
		if (!(ReadFile(
			hNamePipe,//핸들값
			recvMessage,//버퍼이름
			sizeof(recvMessage) - sizeof(TCHAR) * 1,//버퍼사이즈
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
		//sendSize -> NULL 포함한 바이트 수
		//ReadFile을 통해서 클라이언트 파일 읽어들이기
		if (!(WriteFile(
			hNamePipe,//핸들값
			sendMessage,//버퍼이름
			(_tcslen(sendMessage) + 1) * sizeof(TCHAR),//버퍼크기
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
	HANDLE hNamePipe;//이름있는 파이프 선언

	_tprintf(_T("==== Client ==== \n"));
	while (1)
	{
		// 파이프 이름으로 핸들 열기
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
	CloseHandle(hNamePipe);//usange카운터 반환
	return 0;
}

