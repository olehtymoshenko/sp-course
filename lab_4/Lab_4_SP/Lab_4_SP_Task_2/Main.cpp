#define _CRT_SECURE_NO_WARNINGS 
#include "Main.h"

using namespace std;

int main(int argc, char* argv[])
{
	if (argc == 2)
	{
		int countThreads = atoi(argv[1]); // convert string argument to int 
		if (countThreads > 0 && countThreads <= MAX_COUNT_THREADS)
		{
			// create array for containing array for each new thread and 
			// generate numbers in each arrays
			int** arrOfArraysForNewThread;
			arrOfArraysForNewThread = (int**)malloc(sizeof(int**) * countThreads);

			for (int i = 0; i < countThreads; i++)
			{
				arrOfArraysForNewThread[i] = (int*)malloc(sizeof(int) * ARRAY_SIZE);
				GenereteArrayNums(arrOfArraysForNewThread[i]);
			}

			InitializeCriticalSection(&gCriticalSection);
			// alloc index in tls memory
			gdwTlsIndex = TlsAlloc();
			// alloc memory for new threads handles 
			threadHandles = (HANDLE*)malloc(sizeof(HANDLE) * countThreads);
			// alloc memory for ID of new threads
			DWORD* dwThreadsID = (DWORD*)malloc(sizeof(DWORD) * countThreads);

			for (int i = 0; i < countThreads; i++) // create new threads
			{
				threadHandles[i] = CreateThread(NULL,
					NULL,
					StartNewThread,
					arrOfArraysForNewThread[i],
					NULL,
					&dwThreadsID[i]);
			}
			// wait until all threads complete
			WaitForMultipleObjects(countThreads, threadHandles, TRUE, INFINITE);

		}
	}
	else
	{
		cout << "Wrong count of arguments\n";
	}
}

DWORD WINAPI StartNewThread(LPVOID param)
{
	EnterCriticalSection(&gCriticalSection); // only 1 thread in one moment can execute this code
	int maxDivisors[ARRAY_SIZE] = { 1 }; // create memory for array of max divisors
	TlsSetValue(gdwTlsIndex, maxDivisors); // save pointer to array in tls memory

	cout << "---------------------------------------------\n";
	cout << "Thread ID:" << GetCurrentThreadId() << endl;
	cout << "Generated array:";
	PrintArrayToStdout((int*)param, ARRAY_SIZE); 
	cout << endl;
	for (int i = 0; i < ARRAY_SIZE; i++) // calculate largest divisor for each number
	{
		maxDivisors[i] = CalcLargestDivisor(((int*)param)[i]);
	}
	cout << "Max divisors:   ";
	PrintArrayToStdout((int*)TlsGetValue(gdwTlsIndex), ARRAY_SIZE); // print array of  largest divisors from TLS memory
	cout << endl;
	cout << "Numbers sum:" << ArrNumsSum((int*)param) << endl;
	cout << "---------------------------------------------\n";
	LeaveCriticalSection(&gCriticalSection); // allow other threads to execute this code
	return 0;
}

// Print array to stdout
void PrintArrayToStdout(int* array, int lenght)
{
	for (int i = 0; i < lenght; i++)
		cout << " " << array[i] << " ";
}

// generate array in setted range
void GenereteArrayNums(int* arr)
{
	int randRange = ARRAY_NUM_MAX_BOUNDARY - ARRAY_NUM_MIN_BOUNDARY;
	// array generation
	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		arr[i] = (rand() % randRange) + ARRAY_NUM_MIN_BOUNDARY;
	}
}

// calc largest divisor
int CalcLargestDivisor(int num)
{
	int halfNum = ceil(num / 2), res = 1;
	for (int i = 2; i <= halfNum; i++)
	{
		if (num % i == 0)
		{
			res = i;
		}
	}
	return res;
}

// calc array nums sum
int ArrNumsSum(int * arr)
{
	int sum = arr[0];
	for (int i = 1; i < ARRAY_SIZE; i++)
	{
		sum += arr[i];
	}
	return sum;
}
