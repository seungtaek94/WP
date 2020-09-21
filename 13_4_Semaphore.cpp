//  세마포어를 이용한 쓰레드 동기화.
/*
	1. 테이블이 총 10개 == 동시에 총 10명의 손님만 받을 수 있다고 가정
	2. 총 예상 손님 수는 50
	3. 각 손님들이 식사하는데 걸리는 시간은 10~30분 사이
*/

#include "stdio.h"
#include "windows.h"
#include "tchar.h"
#include "process.h"
#include "time.h"

#define NUM_OF_CUSTOMER 50
#define RANGE_MIN 10
#define RANGE_MAX (30 - RANGE_MIN)
#define TABLE_CNT 10

HANDLE hSemaphore;
DWORD randTimeArr[50];

void TakeMeal(DWORD time)
{
	WaitForSingleObject(hSemaphore, INFINITE); // 세마포어 값을 하나 감소시키며 임계 영역에 돌입
	_tprintf(_T("Enter Customer %d~ \n"), GetCurrentThreadId());
	_tprintf(_T("Customer %d having launch~ \n"), GetCurrentThreadId());
	Sleep(1000 * time);

	ReleaseSemaphore(hSemaphore, 1, NULL); // 세마 포어 값을 하나 증가시키며 임계 영역을 빠져 나옴
	_tprintf(_T("Out Customer %d \n\n"), GetCurrentThreadId());
}

unsigned int WINAPI ThreadProc(LPVOID lpParam)
{
	TakeMeal((DWORD)lpParam);
	return 0;
}

int _tmain(int argc, TCHAR* argv[])
{
	DWORD dwThreadID[NUM_OF_CUSTOMER];
	HANDLE hThread[NUM_OF_CUSTOMER];

	// 랜덤 값 생성(손님들 식사에 걸리는 시간 10~30(분) 사이)
	srand((unsigned)time(NULL));
	for (int i = 0; i < NUM_OF_CUSTOMER; i++)
	{
		randTimeArr[i] = (DWORD)(
			((double)rand() / (double)RAND_MAX) * RANGE_MAX + RANGE_MIN
		);
	}
	

	// Semaphore 생성
	hSemaphore = CreateSemaphore(
		NULL,		// default 보안 관리자
		TABLE_CNT,	// 세마포어 초기값
		TABLE_CNT,	// 세마포어 최대값
		NULL		// unnamed 세마포어 구성
	);

	if (hSemaphore == NULL)
	{
		_tprintf(_T("CreateSemaphore error %d \n"), GetLastError());
		return -1;
	}

	for (int i = 0; i < NUM_OF_CUSTOMER; i++)
	{
		hThread[i] = (HANDLE)
			_beginthreadex(
				NULL, 0,
				ThreadProc,
				(void*)randTimeArr[i],
				CREATE_SUSPENDED,
				(unsigned*)& dwThreadID[i]
			);
		if (hThread[i] == NULL)
		{
			_tprintf(_T("Thread Creation Fault! \n"));
			return -1;
		}
	}

	for (int i = 0; i < NUM_OF_CUSTOMER; i++)
	{
		ResumeThread(hThread[i]);
	}

	WaitForMultipleObjects(
		NUM_OF_CUSTOMER, hThread,TRUE, INFINITE
	);

	_tprintf(_T("---------END----------\n"));

	for (int i = 0; i < NUM_OF_CUSTOMER; i++)
	{
		CloseHandle(hThread[i]);
	}

	CloseHandle(hSemaphore);
	return 0;
}