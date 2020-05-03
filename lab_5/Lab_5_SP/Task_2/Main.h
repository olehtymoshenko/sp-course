#include <stdio.h>
#include <iostream>
#include "windows.h" 
#include <ctime>
#include "tchar.h"

#define MIN_THREAD_SLEEPING_TIME_IN_SECONDS 1
#define MAX_THREAD_SLEEPING_TIME_IN_SECONDS 3
#define FILE_HANDLES_COUNT 3
#define BUFF_FOR_WRITING_TO_LOG_FILE_SIZE 32

// func for start new threads
DWORD WINAPI StartNewThread(LPVOID param);

// struct for contain params for new threads
typedef struct {
	DWORD id;
	DWORD sleepingTime;
} NewThreaParams, * LPNewThreadParams;

// global variables
HANDLE hFilesAccessSemaphore;
CONST TCHAR ctsFileForSaving[] = L"log.txt";