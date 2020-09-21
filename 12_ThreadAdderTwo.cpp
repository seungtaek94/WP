// Thread 종료시  return을 이용하면 좋은 경우(대부분의 경우)
// 1~10 까지를 더하되 스레드 3개를 생성해 분할하여 더함(전역변수이용)

#include "stdio.h"
#include "tchar.h"
#include "windows.h"


// 스레드는 Code, Heap, Data 영역을 공유하기 때문에 각 스레드에서 전역변수에 접근할 수 있다.
// 그러나, 둘 이상의 스레드에서 동시에 접근가능하기 때문에 동기화 문제가 발생할 수 있다.
static int total = 0;

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	DWORD* nPtr = (DWORD*)lpParam;

	DWORD numOne = *nPtr;
	DWORD numTwo = *(nPtr + 1);

	for (DWORD i = numOne; i <= numTwo; i++)
	{
		total += i;
	}
	return 0;
}

int _tmain(int argc, TCHAR* argv[])
{
	DWORD dwThreadID[3];
	HANDLE hTread[3];

	DWORD paramThread[] = { 1, 3, 4, 7, 8, 10 };
	
	hTread[0] =
		CreateThread(
			NULL, 0,
			ThreadProc,
			(LPVOID)(&paramThread[0]),
			0, &dwThreadID[0]
		);
	hTread[1] =
		CreateThread(
			NULL, 0,
			ThreadProc,
			(LPVOID)(&paramThread[2]),
			0, &dwThreadID[1]
		);
	hTread[2] =
		CreateThread(
			NULL, 0,
			ThreadProc,
			(LPVOID)(&paramThread[4]),
			0, &dwThreadID[2]
		);

	if (hTread[0] == NULL || hTread[1] == NULL || hTread[2] == NULL)
	{
		_tprintf(_T("thread creation fault!\n"));
		return -1;
	}

	WaitForMultipleObjects(
		3,			// 3개의 커널오브젝트를 관찰
		hTread,		// 커널 오브젝트 핸들의 배열 정보
		TRUE,		// 모든 커널 오브젝트가 signaled 상태가 되기를
		INFINITE	// 영원히 기다림
	);

	_tprintf(_T("total (1~10): %d \n"), total);

	CloseHandle(hTread[0]);
	CloseHandle(hTread[1]);
	CloseHandle(hTread[2]);
	return 0;
}