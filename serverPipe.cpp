#include <stdio.h>
#include <Windows.h>//윈도우플랫폼 헤더
#include <tchar.h>//유니코드 멀티바이트 사용하기위한 헤더
#pragma warning(disable: 4996)//scanf에러처리

#define PIPE_NAME	"\\\\.\\pipe\\test_pipe"     
// 파이프 이름 설정, 이 파이프로 통신을 할 것임

//handle값이 파라미터인 클라이언트와 연결시켜주는 함수
int ConnectClient(HANDLE hNamePipe)
{
	TCHAR recvMessage[100];//받을 메세지
	TCHAR sendMessage[100];//보낼 메세지
	DWORD recvSize;//받을 메세지 크기
	DWORD sendSize;//보낼 메세지 크기
	TCHAR value[1024]; //환경변수 크기

	TCHAR command[] = _T("Client"); //서버 환경변수 설정

	STARTUPINFO si = { 0, };//process의 인자값 초기화 변수
	PROCESS_INFORMATION pi = { 0, };//process의 인자값 초기화 변수

	SetEnvironmentVariable(command, _T("Connet"));//환경변수 셋
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
		_tprintf("Server 환경변수 체크 완료!\n");

	while (1)
	{
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
		)))          // 
		{
			//예외처리
			_tprintf(_T("WriteFile error! \n"));
			return -1;
		}
		FlushFileBuffers(hNamePipe);
		//recvSize -> NULL 포함한 바이트 수
		if (!(ReadFile(
			hNamePipe,//핸들값
			recvMessage,//버퍼이름
			sizeof(recvMessage) - sizeof(TCHAR) * 1,//버퍼크기
			&recvSize,
			NULL
		)))
		{
			//예외처리
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
	HANDLE hNamePipe;//이름있는 파이프 선언
	TCHAR pipe_name[] = _T(PIPE_NAME);     // #define 한 파이프 이름
	_tprintf(_T("==== Server ==== \n"));
	//NamedPipe 생성
	hNamePipe = CreateNamedPipe(
		pipe_name,//lpName
		PIPE_ACCESS_DUPLEX,//dwOpenMode
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,//dwPipeMode
		PIPE_UNLIMITED_INSTANCES,//nMaxInstances
		0,//nOutBufferSize
		0,//nInBufferSize
		20000,       // 대기 Timeout 시간
		NULL//lpSecurityAttributes
	);
	//에러처리
	if (hNamePipe == INVALID_HANDLE_VALUE)
	{
		printf("CreateNamePipe error! \n");
		return -1;
	}

	while (1)
	{
		//생성한 Named Pipe의 핸들을 누군가 얻어갈 때까지 대기
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
	DisconnectNamedPipe(hNamePipe);//연결끊기
	CloseHandle(hNamePipe);//usage카운터 반환
	_tprintf(_T("Program exit! \n"));
	return 1;
}