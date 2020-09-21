// InterLocked을 이용한 쓰레드 동기화.

#include "stdio.h"
#include "windows.h"
#include "tchar.h"
#include "process.h"

#define NUM_OF_GATE 6

LONG gTotalCount = 0;

void IncreaseCount()
{
	//gTotalCount++;	// Critical Section(임계 영역)
	InterlockedIncrement(&gTotalCount); // 값을 증가 실킬 32 bit 변수의 주소값을 전달.
}

unsigned int WINAPI ThreadProc(LPVOID lpParam)
{
	for (DWORD i = 0; i < 1000; i++)
	{
		IncreaseCount();
	}
	return 0;
}

int _tmain(int argc, TCHAR* argv[])
{
	DWORD dwThreadID[NUM_OF_GATE];
	HANDLE hTread[NUM_OF_GATE];

	for (DWORD i = 0; i < NUM_OF_GATE; i++)
	{
		hTread[i] = (HANDLE)
			_beginthreadex(
				NULL, 0,
				ThreadProc,
				NULL,
				CREATE_SUSPENDED,
				(unsigned*)&dwThreadID[i]
			);

		if (hTread[i] == NULL)
		{
			_tprintf(_T("Thread creation fault! \n"));
			return -1;
		}
	}

	for (DWORD i = 0; i < NUM_OF_GATE; i++)
	{
		ResumeThread(hTread[i]);
	}

	WaitForMultipleObjects(
		NUM_OF_GATE, hTread, TRUE, INFINITE
	);

	_tprintf(_T("total count: %d \n"), gTotalCount);

	for (DWORD i = 0; i < NUM_OF_GATE; i++)
	{
		CloseHandle(hTread[i]);
	}

	return 0;
}