#include <stdio.h>
#include "iostream"
#include "windows.h"
#include <ctime>

#define ARRAY_SIZE 10000000 // array size which will be created for calculating, 10kk
#define ARRAY_NUM_MIN_BOUNDARY 0 // low bound for rand array nums
#define ARRAY_NUM_MAX_BOUNDARY 100000 // high bound for rand arrray nums

#define SUM_OPERATION 0
#define BUBBLE_SORT_OPERATION 1
#define AVARAGE_OPERATION 2

////////////////// functions prototypes 
DWORD WINAPI StartNewThreadWithCriticalSection(LPVOID param);
DWORD WINAPI StartNewThreadWithoutCriticalSection(LPVOID param);
void GenereteArrayNums(int* arr); // generate numbers for array
int ArrNumsSum(int* arr); // calculate array numbers sum
void BubbleSort(int* arr);
LONG64 CalcAvarageValue(int* arr);

/////////////////// global variables
CRITICAL_SECTION gCriticalSection; // critical section
HANDLE* threadHandles; // handles of creat threads
CONST DWORD COUNT_THREADS = 3; // count threads
int arrayForCalculating[ARRAY_SIZE]{ 0 }; // array for calculating