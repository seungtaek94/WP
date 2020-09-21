// main 쓰레드에서 입력을 받으면 또다른 쓰레드를 통해 출력.
// -> 입력을 받을 때까지 출력하는 쓰레드는 기달려야 한다는 의미.

#include "stdio.h"
#include "tchar.h"
#include "windows.h"
#include "process.h"

unsigned int WINAPI OutputThreadFunc(LPVOID lpParam);

TCHAR string[100];
HANDLE hEvent;

int _tmain(int argc, TCHAR* argv[])
{
	HANDLE hThread;
	DWORD dwThreadID;

	// 이벤트 생성
	// 수동 모드로 생성을 했으므로
	// Signaled 및 non-Signaled 상태로의 변경은 모두 수동으로 처리해야함
	hEvent = CreateEvent(
		NULL,	// 상속 불가
		TRUE,	// manual-reset mode로 생성
		FALSE,	// non-signaled 상태로 생성
		NULL	// 이름 없는 event
	);

	if (hEvent == NULL)
	{
		_fputts(_T("Event object creation error \n"), stdout);
		return -1;
	}

	hThread = (HANDLE)_beginthreadex(
		NULL, 0,
		OutputThreadFunc,
		NULL, 0,
		(unsigned*)&dwThreadID
	);

	if (hThread == 0)
	{
		_fputts(_T("Thread creation error \n"), stdout);
		return -1;
	}

	_fputts(_T("Insert string: "), stdout);
	_fgetts(string, 30, stdin);

	SetEvent(hEvent);	// 입력을 받았으므로 event의 state를 signaled 상태로 변경

	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hEvent);	// event 오브젝트 소멸
	CloseHandle(hThread);

	return 0;
}

unsigned int WINAPI OutputThreadFunc(LPVOID lpParam)
{
	// event가 signaled 상태가 되기를 기다림
	WaitForSingleObject(hEvent, INFINITE);

	_fputts(_T("Output string: "), stdout);
	_fputts(string, stdout);

	return 0;
}