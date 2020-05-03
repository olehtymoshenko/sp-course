#include <stdio.h>
#include "iostream"
#include "windows.h"

#define ARRAY_SIZE 5 // array size which will be created for calculating
#define MAX_COUNT_THREADS 100 // max count of creating threads
#define ARRAY_NUM_MIN_BOUNDARY 10 // low bound for rand array nums
#define ARRAY_NUM_MAX_BOUNDARY 100 // high bound for rand arrray nums

////////////////// functions prototypes 
DWORD WINAPI StartNewThread(LPVOID param); // start new thread
void GenereteArrayNums(int* arr); // generate numbers for array
int CalcLargestDivisor(int num); // calculate max divisor
int ArrNumsSum(int* arr); // calculate array numbers sum
void PrintArrayToStdout(int* array, int lenght); // output array to stdout

/////////////////// global variables
int gdwTlsIndex; // containt index for Tls memeory
CRITICAL_SECTION gCriticalSection; // critical section
HANDLE* threadHandles; // handles of creat threads