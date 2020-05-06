#include "Task_3.h"

using namespace std;

int main(int argc, char* argv[])
{
	clock_t start, end;

	InitializeCriticalSection(&gCriticalSection);

	// alloc memory for new threads handles 
	threadHandles = (HANDLE*)malloc(sizeof(HANDLE) * COUNT_THREADS);

	GenereteArrayNums(arrayForCalculating); // generate array nums

	// start with using critical section --------------------------------------------------------------- first mode 
	cout << " *** Start calculating with critical section ***\n";
	start = clock();
	for (int i = 0; i < COUNT_THREADS; i++) // create new threads
	{
		threadHandles[i] = CreateThread(NULL,
			NULL,
			StartNewThreadWithCriticalSection,
			(LPVOID)i,
			NULL,
			NULL);
	}
	// wait until all threads complete
	WaitForMultipleObjects(COUNT_THREADS, threadHandles, TRUE, INFINITE);
	end = clock();
	cout << "Calculating with critical section continued for " << end - start << " ms.\n";
	cout << "--------------------------------------------------------------------\n";
	// start without using critical section ------------------------------------------------------------- second mode
	cout << " *** Start calculating without critical section ***\n";
	start = clock();
	for (int i = 0; i < COUNT_THREADS; i++) // create new threads
	{
		threadHandles[i] = CreateThread(NULL,
			NULL,
			StartNewThreadWithoutCriticalSection,
			LPVOID(i),
			NULL,
			NULL);
	}
	// wait until all threads complete
	WaitForMultipleObjects(COUNT_THREADS, threadHandles, TRUE, INFINITE);
	end = clock();
	cout << "Calculating without critical section continued for " << end - start << " ms.\n";

	for (int i = 0; i < COUNT_THREADS; i++)
		CloseHandle(threadHandles[i]);
	DeleteCriticalSection(&gCriticalSection);
	return 0;
}

// calculating with using critical section 
DWORD WINAPI StartNewThreadWithCriticalSection(LPVOID param)
{
	switch ((int)param)
	{
	case SUM_OPERATION:
	{
		EnterCriticalSection(&gCriticalSection);
		ArrNumsSum(arrayForCalculating);
		LeaveCriticalSection(&gCriticalSection);
	} break;
	case BUBBLE_SORT_OPERATION:
	{
		EnterCriticalSection(&gCriticalSection);
		BubbleSort(arrayForCalculating);
		LeaveCriticalSection(&gCriticalSection);
	} break;
	case AVARAGE_OPERATION:
	{
		EnterCriticalSection(&gCriticalSection);
		CalcAvarageValue(arrayForCalculating);
		LeaveCriticalSection(&gCriticalSection);
	} break;
	default:
		break;
	}
	return 0;
}

// calculating without using critical section 
DWORD WINAPI StartNewThreadWithoutCriticalSection(LPVOID param)
{
	switch ((int)param)
	{
	case SUM_OPERATION:
	{
		ArrNumsSum(arrayForCalculating);
	} break;
	case BUBBLE_SORT_OPERATION:
	{
		BubbleSort(arrayForCalculating);
	} break;
	case AVARAGE_OPERATION: 
	{
		CalcAvarageValue(arrayForCalculating);
	} break;
	default:
		break;
	}
	return 0;
}


// generate array in setted range
void GenereteArrayNums(int* arr)
{
	DWORD randRange = ARRAY_NUM_MAX_BOUNDARY - ARRAY_NUM_MIN_BOUNDARY;
	// array generation
	for (DWORD i = 0; i < ARRAY_SIZE; i++)
	{
		arr[i] = (rand() % randRange) + ARRAY_NUM_MIN_BOUNDARY;
	}
}

// Calculate avarage value of array
LONG64 CalcAvarageValue(int* arr)
{
	LONG64 res = 0;
	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		res += arr[i];
	}
	res /= ARRAY_SIZE;
	cout << "Avarage:" << res << endl;
	return res;
}

// array sort using Bubble sort algorithm
void BubbleSort(int* arr)
{

	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		for (int j = 0; i < ARRAY_SIZE - i - 1; i++)
		{
			if (arr[j] > arr[j + 1]) swap(arr[j], arr[j]); // fake swap elements for preserving the initial state
														   // of the array for fair comparison execution with 
														   // and without critical section
		}
	}
	cout << "First:" << arr[0] << ";last:" << arr[ARRAY_SIZE - 1] << endl;
	return;
}

// calc array nums sum
int ArrNumsSum(int* arr)
{
	int sum = arr[0];
	for (int i = 1; i < ARRAY_SIZE; i++)
	{
		sum += arr[i];
	}
	cout << "Sum:" << sum << endl;;
	return sum;
}
