// Thread 종료시  return을 이용하면 좋은 경우(대부분의 경우)
// 1~10 까지를 더하되 스레드 3개를 생성해 분할하여 더함
#include "stdio.h"
#include "windows.h"
#include "tchar.h"

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	DWORD *nPtr = (DWORD*)lpParam;
	
	DWORD numOne = *nPtr;
	DWORD numTwo = *(nPtr + 1);

	DWORD total = 0;

	for (DWORD i = numOne; i <= numTwo; i++)
	{
		total += i;
	}

	return total;
}

int _tmain(int argc, TCHAR* argv[])
{
	DWORD dwThreadID[3];
	HANDLE hTread[3];

	DWORD paramThread[] = { 1, 3, 4, 7, 8, 10 };
	DWORD total = 0;
	DWORD result = 0;

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

	GetExitCodeThread(hTread[0], &result);
	total += result;
	GetExitCodeThread(hTread[1], &result);
	total += result;
	GetExitCodeThread(hTread[2], &result);
	total += result;

	_tprintf(_T("total (1~10): %d \n"), total);

	CloseHandle(hTread[0]);
	CloseHandle(hTread[1]);
	CloseHandle(hTread[2]);
	return 0;
}