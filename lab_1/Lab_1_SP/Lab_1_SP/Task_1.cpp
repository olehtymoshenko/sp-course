// declaration part
#include <stdio.h>
#include "windows.h"
#include "iostream"
#include <locale.h>

using namespace std;

#define MAX_ARGUMENT_LENGTH 2
// functions for implementation -s argument
void ImplementationShowErrorArg();
void ImplementationShowSystemInfoArg();
void StartWithoutArgs();
void GlobalMemoryInfoPrint();
void GetSystemInfoPrint();

// functions for implementation -e argument
bool ThrowOutOfMemoryError();
void ShowError();


// implementation part

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "rus");

    if (argc != 2)
    {
        StartWithoutArgs();
    }
    else
    {
        if (strcmp(argv[1], "-e") == 0)
        {
            ImplementationShowErrorArg();
        }
        else if (strcmp(argv[1], "-s") == 0)
        {
            ImplementationShowSystemInfoArg();
        }
        else // error argument
        {
            StartWithoutArgs();
        }
    }
    return 0;
}

void ImplementationShowErrorArg()
{
    // call function for print error 
    if (ThrowOutOfMemoryError())
    {
        ShowError();
    }
    else
    {
        printf("Sorry, you have too much free memory");
    }
}

void ImplementationShowSystemInfoArg()
{
    // call function for  print information about system
    GlobalMemoryInfoPrint();
    GetSystemInfoPrint();
}

bool ThrowOutOfMemoryError()
{
    DWORD userMemCount = 0;
    MEMORYSTATUS memStatus;
    GlobalMemoryStatus(&memStatus);
    HLOCAL pAllocMem = LocalAlloc(LMEM_FIXED, memStatus.dwTotalVirtual + 5000000000); // total virtual memory + 5 GB
    if (pAllocMem == NULL)
    {
        return true;
    }
    else
    {
        LocalFree(pAllocMem);
        return false;
    }
}

void ShowError()
{
    LPVOID lpMsgBuf;
    DWORD dwLastError = GetLastError();
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, dwLastError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)& lpMsgBuf, 0, NULL);
    printf("\n%s\n", lpMsgBuf);
    LocalFree(lpMsgBuf);
    return;
}

void StartWithoutArgs()
{
    printf("You don`t input argument or input wrong argument. Programm supports 2 arguments:\n");
    printf("'-e' - print error\n");
    printf("'-s' - print information about system\n\n");
    exit(EXIT_FAILURE);
}

// Func for GlobalMemoryStatus() information print
void GlobalMemoryInfoPrint()
{
    MEMORYSTATUS memStatusStruct;
    GlobalMemoryStatus(&memStatusStruct);
    cout << "   *** information about the current state of both physical and virtual memory ***" << endl;
    cout << "Approximate percentage of physical memory that is in use:"
        << memStatusStruct.dwMemoryLoad << endl;
    // phisical memory
    cout << "The  amount of actual physical memory, in bytes:"
        << memStatusStruct.dwTotalPhys << endl;
    cout << "The amount of physical memory currently available, in bytes:"
        << memStatusStruct.dwAvailPhys << endl;
    // memory limit for process
    cout << "The current size of the committed memory limit, in bytes:"
        << memStatusStruct.dwTotalPageFile << endl;
    cout << "The maximum amount of memory the current process can commit, in bytes:"
        << memStatusStruct.dwAvailPageFile << endl;
    // virtual memory
    cout << "The total amount of virtual memory, in bytes:"
        << memStatusStruct.dwTotalVirtual << endl;
    cout << "The amount of availvable virtual memory, in bytes:"
        << memStatusStruct.dwAvailVirtual << endl;

    cout << "\nUsed function is GlobalMemoryStatus()\n" << endl;
}

// Func for GetSystemInfo information print
void GetSystemInfoPrint()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    printf("    *** Hardware information ***\n");
    printf("  Processor architecture: %u\n",       sysInfo.wProcessorArchitecture);
    printf("  Processor type: %u\n",               sysInfo.dwProcessorType);
    printf("  Number of processors: %u\n",         sysInfo.dwNumberOfProcessors);
    printf("  Active processor mask: %u\n",        sysInfo.dwActiveProcessorMask);
    printf("  Page size: %u\n",                    sysInfo.dwPageSize);
    printf("  Minimum application address: %p\n",  sysInfo.lpMinimumApplicationAddress);
    printf("  Maximum application address: %p\n",  sysInfo.lpMaximumApplicationAddress);
    printf("  The granularity for the starting" 
           "  address at which virtual"
           "  memory can be allocated: %u\n",      sysInfo.dwAllocationGranularity);
    printf("\nUsed function is GetSystemInfo()\n");
}