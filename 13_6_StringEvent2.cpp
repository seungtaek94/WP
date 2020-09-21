// main 쓰레드에서 입력을 받으면 
// 또다른 쓰레드를 통해 입력받은 문자열 출력
// 또다른 쓰레드를 통해 문자열의 길이를 출력
// -> 입력을 받을 때까지 문자열을 출력하는 쓰레드와 문자열의 길이를 출력하는 쓰레드를 기달려야 한다는 의미.
// -> 이 프로그램은 두개의 자식 쓰레드의 순서가 뒤바뀔수 있는 문제를 가지고 있다.
#include "stdio.h"
#include "tchar.h"
#include "windows.h"
#include "process.h"

unsigned int WINAPI OutputThreadFunc(LPVOID lpParam);
unsigned int WINAPI CountThreadFunc(LPVOID lpParam);

TCHAR string[100];
HANDLE hEvent;

int _tmain(int argc, TCHAR* argv[])
{
	HANDLE hThread[2];
	DWORD dwThreadID[2];

	// 이벤트 오브젝트 생성
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

	hThread[0] = (HANDLE)_beginthreadex(
		NULL, 0,
		OutputThreadFunc,
		NULL, 0,
		(unsigned*) &dwThreadID[0]
	);

	hThread[1] = (HANDLE)_beginthreadex(
		NULL, 0,
		CountThreadFunc,
		NULL, 0,
		(unsigned*) &dwThreadID[1]
	);

	if (hThread[0] == 0 || hThread[1] == 0)
	{
		_fputts(_T("Thread creation error \n"), stdout);
		return -1;
	}

	_fputts(_T("Insert string: "), stdout);
	_fgetts(string, 30, stdin);

	SetEvent(hEvent);	// 입력을 받았으므로 event의 state를 signaled 상태로 변경

	WaitForMultipleObjects(
		2,			// 배열의 길이
		hThread,	// 핸들의 배열
		TRUE,		// 모든 핸들이 신호받은 상태로 될때 반환
		INFINITE	// 무한 대기
	);

	CloseHandle(hEvent);	// event 오브젝트 소멸
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);

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

unsigned int WINAPI CountThreadFunc(LPVOID lpParam)
{
	WaitForSingleObject(hEvent, INFINITE);
	_tprintf(_T("Output string length: %d \n"), _tcslen(string) - 1);

	return 0;
}