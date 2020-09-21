// 둘 이상의 쓰레드가 동시에 실행할 경우 문제가 발생할 수 있고,
// 이러한 문제를 이르키는 "코드블록"을 가리켜 "Critical Section" 이라함.

/* 
아래의 코드에서 의도한 바는 각 스레드가 1000번씩 gTotalCount를 증가시켜 총 6000의 값을 증가 시키는 것이다. 
그러나 실제로 실행시켜 보면 그렇지 않은 경우가 발생함을 알 수 있다.
*/

#include "stdio.h"
#include "windows.h"
#include "tchar.h"
#include "process.h"

#define NUM_OF_GATE 6

LONG gTotalCount = 0;

void IncreaseCount()
{
	gTotalCount++;	// Critical Section(임계 영역)
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
				(unsigned*) &dwThreadID[i]
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