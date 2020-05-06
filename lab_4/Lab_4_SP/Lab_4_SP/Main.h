#include <stdio.h>
#include "iostream"
#include "windows.h"
#include  "tchar.h"

#define READ_PIPE_BUFF_SIZE 1024

#define ADD_TO_ARCHIVE_MENU_OPERATION 0
#define EXTRACT_FILES_MENU_OPERATION 1
#define EXIT_MENU_OPERATION 2

///// functions delcaration
void PrintMenu();
void StartArchiving(TCHAR* filePath, TCHAR* archPath);
void StartExtracting(TCHAR* archPath, TCHAR* extractArchPath);
void ShowError();
void CreateNew7zipProcess(TCHAR* cmdForNewProc);