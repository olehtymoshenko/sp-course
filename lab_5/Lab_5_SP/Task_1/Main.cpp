#include <stdio.h>
#include "windows.h"


int main()
{
	// name of named mutex
	TCHAR sMutexName[]{ L"OneInstanceMutex" };
	
	// try to open mutex
	HANDLE hOneInstMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, sMutexName);

	// if cant open mutex, than create it 
	if (!hOneInstMutex)
	{
		hOneInstMutex = CreateMutex(NULL, TRUE, sMutexName);
		wprintf(L"Is Running");
		while (1) { }
	}
	else // if opening succesed than app intance already run
	{
		wprintf(L"Only 1 app intance available \n");
		Sleep(5000);
	}
}