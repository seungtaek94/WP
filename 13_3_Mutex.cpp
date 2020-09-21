//  Mutex를 이용한 쓰레드 동기화.

#include "stdio.h"
#include "windows.h"
#include "tchar.h"
#include "process.h"

#define NUM_OF_GATE 6

LONG gTotalCount = 0;

// Mutex는 커널 오브젝트이므로 HANDLE 타입 변수에 핸들값을 저장해 사용.
HANDLE hMutex;

DWORD AcquireMutex(HANDLE mutex)
{
	return WaitForSingleObject(mutex, INFINITE);
}

void IncreaseCount()
{
	// WaitForSingleObject 함수는 다양한 용도로 사요됨
	// 따라서, 가독성을 위해 함수를 Wrapping해서 사용하는 것을 추천

	//WaitForSingleObject(hMutex, INFINITE);
	AcquireMutex(hMutex);
	gTotalCount++;	// Critical Section(임계 영역)
	ReleaseMutex(hMutex);
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

	// mutex 생성, 누구에게 소유되지 않은 상태(Signaled 상태)로 생성
	hMutex = CreateMutex(
		NULL,	// default 보안 관리자
		FALSE,	// 누구나 소유할 수 있는 상태로 생성
		NULL	//nonamed mutex
	);

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

	// 할당 받은 mutex 리소스를 해제, 호출하는 시점에서 할당된 커널 오브젝트가 소멸됨
	CloseHandle(hMutex);

	return 0;
}