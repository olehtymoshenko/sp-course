#include "Main.h"

using namespace std;

int main()
{
	HANDLE* hThreads; // threads handles
	
	// get count threads
	DWORD dwCountThreads;
	cout << "Count of file handles:" << FILE_HANDLES_COUNT << endl;
	cout << "Input count of threads:\n";
	scanf_s("%d", &dwCountThreads);

	// alloc memory for new threads handles
	hThreads = (HANDLE*)malloc(sizeof(HANDLE) * dwCountThreads);
	
	// create counter semaphore for control count handles to file
	hFilesAccessSemaphore = CreateSemaphore(NULL, FILE_HANDLES_COUNT, FILE_HANDLES_COUNT, L"FilesSemaphore");
	if (!hFilesAccessSemaphore || hFilesAccessSemaphore == INVALID_HANDLE_VALUE)
	{
		cout << "Error! Cant create semaphore\n";
		return 1;
	}
	
	DWORD dwRandRange = MAX_THREAD_SLEEPING_TIME_IN_SECONDS - MIN_THREAD_SLEEPING_TIME_IN_SECONDS + 1;
	// start all threads
	for (int i = 0; i < dwCountThreads; i++)
	{
		// srand using current time for get more random number
		srand(clock());
		NewThreaParams param = { 0 }; // struct for containing params for new thread
		param.id = i; // set user id
		param.sleepingTime = (rand() % dwRandRange) + MIN_THREAD_SLEEPING_TIME_IN_SECONDS; // rand time to sleeping
		hThreads[i] = CreateThread(NULL, NULL, StartNewThread, (LPVOID)&param, NULL, NULL); 
		if ((!hThreads[i]) || (hThreads[i] == INVALID_HANDLE_VALUE))
		{
			cout << "Error, cant start thread number: " << i << endl;
			return 2;
		}
		Sleep(100); // for srand, for more differance in time between each rand()
	}

	WaitForMultipleObjects(dwCountThreads, hThreads, TRUE, INFINITE); // wait until all thread completed

	// close all handles
	CloseHandle(hFilesAccessSemaphore); 
	for (int i = 0; i < dwCountThreads; i++)
		CloseHandle(hThreads[i]);
	free(hThreads);

	CloseHandle(hFilesAccessSemaphore);
	cout << "All threads complete\n";
	getchar();
	return 0;
}

// func for creating new thread
DWORD WINAPI StartNewThread(LPVOID param)
{
	// save params in stack
	DWORD dwThreadUserId = ((LPNewThreadParams)param)->id;
	DWORD dwThreadUserSleepingTime = ((LPNewThreadParams)param)->sleepingTime;

	HANDLE hFileForSaving; // handle to file for saving info
	DWORD dwWaitingRes = ERROR_SUCCESS;
	clock_t  startTime = clock(), endTime;

	// wait for resourse (handle to file)
	WaitForSingleObject(hFilesAccessSemaphore, INFINITE);

	printf_s("Thread %d get resourse\n", dwThreadUserId);

	printf_s("Thread %d will sleep for %d sec\n", dwThreadUserId, dwThreadUserSleepingTime);
	Sleep(dwThreadUserSleepingTime * 1000); 

	// create handle to file
	if ((hFileForSaving = CreateFile(ctsFileForSaving,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL)) == INVALID_HANDLE_VALUE)
	{
		cout << "Error from thread " << dwThreadUserId << ", cant open file\n";
		ReleaseSemaphore(hFilesAccessSemaphore, 1, NULL);
		ExitThread(1);
	}
	
	// set file pointer to end
	if ((dwWaitingRes = SetFilePointer(hFileForSaving, 0, NULL, FILE_END)) == INVALID_SET_FILE_POINTER)
	{
		cout << "Error from thread " << dwThreadUserId << ", cant set a file pointer in the file\n";
		ReleaseSemaphore(hFilesAccessSemaphore, 1, NULL);
		ExitThread(2);
	}
	endTime = clock();

	// buff for writing to file
	TCHAR buff[BUFF_FOR_WRITING_TO_LOG_FILE_SIZE] { '\0' };
	DWORD dwWritenBytes;

	// make string for saving in file
	_sntprintf_s(buff, BUFF_FOR_WRITING_TO_LOG_FILE_SIZE, L"Thread %d ends in %d seconds\n", dwThreadUserId, ((endTime - startTime) / 1000));

	// write to file
	if (!WriteFile(hFileForSaving, buff, _tcslen(buff)*sizeof(TCHAR) , &dwWritenBytes, NULL))
	{
		cout << "Error from thread " << dwThreadUserId << ", cant write into the file\n";
		ReleaseSemaphore(hFilesAccessSemaphore, 1, NULL);
		ExitThread(3);
	}
	// clos handle to file
	CloseHandle(hFileForSaving);
	printf_s("Thread %d complete, bytes writen to file:%d\n", dwThreadUserId, dwWritenBytes);
	
	// allow other thread use resourse (handle to file)
	ReleaseSemaphore(hFilesAccessSemaphore, 1, NULL);
	return 0;
}
