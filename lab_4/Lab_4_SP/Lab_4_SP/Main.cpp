#define _CRT_SECURE_NO_WARNINGS 
#include "Main.h"

using namespace std;

// functions implementation

int main()
{
	short choice = -1;

	for (;;)
	{
		PrintMenu();
		cin >> choice;
		// choose option
		switch (choice)
		{
		case ADD_TO_ARCHIVE_MENU_OPERATION:
		{
			TCHAR pathToFileForArch[MAX_PATH] = { '\0' };
			TCHAR pathToNewArch[MAX_PATH] = { '\0' };

			cout << "Input full path to file for archiving:\n";
			scanf_s("%s", pathToFileForArch, MAX_PATH);
			cout << "Input full path to new archive:\n";
			scanf_s("%s", pathToNewArch, MAX_PATH);
			StartArchiving(pathToFileForArch, pathToNewArch);

		}break;
		case EXTRACT_FILES_MENU_OPERATION:
		{
			TCHAR pathToExtractArch[MAX_PATH] = { '\0' };
			TCHAR pathToExistArch[MAX_PATH] = { '\0' };

			cout << "Input full path to archive:\n";
			scanf_s("%s", pathToExistArch, MAX_PATH);
			cout << "Input full path to directory for extracting:\n";
			scanf_s("%s", pathToExtractArch, MAX_PATH);
			StartExtracting(pathToExistArch, pathToExtractArch);
		} break;
		case EXIT_MENU_OPERATION:
		{
			return 0;
		} break;
		default:
			cout << "Error choice, try again\n";
			break;
		}
	}
	return 0;
}

void StartArchiving(TCHAR* filePath, TCHAR* archPath)
{
	// create a require command for new process
	TCHAR cmdForNewProcess[MAX_PATH] = { '\0' };
	snprintf(cmdForNewProcess, MAX_PATH, "%s%s %s", TEXT("7z.exe a "), archPath, filePath);
	cout << cmdForNewProcess;
	CreateNew7zipProcess(cmdForNewProcess);
}

void StartExtracting(TCHAR* archPath, TCHAR* extractArchPath)
{
	// create a require command for new process
	TCHAR cmdForNewProcess[MAX_PATH] = { '\0' };
	snprintf(cmdForNewProcess, MAX_PATH, "%s%s -o\"%s\"", TEXT("7z.exe x "), archPath, extractArchPath);
	cout << cmdForNewProcess;
	CreateNew7zipProcess(cmdForNewProcess);
}


void CreateNew7zipProcess(TCHAR* cmdForNewProc)
{
	// create pipe
	HANDLE hServer = { NULL };
	HANDLE hClient = { NULL };

	// set default setting and enable inherit 
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	// try to create pipe
	if ((!CreatePipe(&hClient, &hServer, &sa, NULL)) ||
		(hServer == INVALID_HANDLE_VALUE) || (hClient == INVALID_HANDLE_VALUE))
	{
		CloseHandle(hClient);
		CloseHandle(hServer);
		ShowError();
		return;
	}

	// set default settings for STARTUPINFO structa and
	// redirect stderr and stdout to pipe (phServer)
	STARTUPINFO si = {NULL};
	si.cb = sizeof(si);
	si.dwFlags |= STARTF_USESTDHANDLES;
	si.hStdError = hServer;
	si.hStdOutput = hServer;

	PROCESS_INFORMATION pi = { NULL };
	// create process
	if (!CreateProcess(NULL,
		cmdForNewProc,
		NULL,
		NULL,
		TRUE,
		ABOVE_NORMAL_PRIORITY_CLASS,
		NULL,
		NULL,
		&si,
		&pi
	))
	{
		CloseHandle(hClient);
		CloseHandle(hServer);
		ShowError();
		return;
	}

	WaitForSingleObject(pi.hProcess, INFINITE); // wait until the process completes
	TerminateProcess(pi.hProcess, 0); // terminate process 7zip process

	// !!! close server, cause ReadFile will read infinite in while without this
	CloseHandle(hServer);
	// close child process handles
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	DWORD dwReadedBytes = 0;
	TCHAR buff[READ_PIPE_BUFF_SIZE] = { '\0' };
	BOOLEAN isAllRight = TRUE;
	// read all information from pipe
	while (ReadFile(hClient, buff, sizeof(buff)-1, &dwReadedBytes, NULL) || (dwReadedBytes != 0))
	{
		printf("%s", buff);
		if (strstr(buff, "Everything is Ok") == NULL) isAllRight = FALSE;
		memset(buff, '\0', sizeof(buff));
	}
	// child proc strerr or strout processing in parent proc 
	cout << "Main process result (handling a child proc strerr and stdout):\n";
	if (!isAllRight)
	{
		cout << "Error! Operation failed\n";
	}
	else
	{
		cout << "Operation succeeded\n";
	}
	// close client part of pipe
	CloseHandle(hClient);
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

void PrintMenu()
{
	cout << "\t\t Menu\n";
	cout << "\t" << ADD_TO_ARCHIVE_MENU_OPERATION << " - Create archive\n";
	cout << "\t" << EXTRACT_FILES_MENU_OPERATION << " - Extract files from archive\n";
	cout << "\t" << EXIT_MENU_OPERATION << " - Exit\n";
}