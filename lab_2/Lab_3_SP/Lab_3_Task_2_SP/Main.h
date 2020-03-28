#include <iostream>
#include "windows.h"
#include <stdio.h>
#include "shlwapi.h"
#include "TCHAR.h"

#define PATH_MAX_LENGHT (MAX_PATH + 1) 

// func prototypes
void PrintMenu();
bool ChangeDirectory(TCHAR* sCurrDirectory, BOOL isStdinContainNewlineChar);
bool PrintDirectoryContent(TCHAR sCurrDirectory[]);
void ReadPath(TCHAR sBuffNewPath[], BOOL isUsedBeforeInputChar);
bool CreateDirectoryOwn(BOOL isStdinContainNewlineChar);
bool CopyFileOwn(BOOL isStdinContainNewlineChar);
bool DeleteEmptyDirectoryOrFile(BOOL isStdinContainNewlineChar);
bool PrintDetailedInfoAboutFile(BOOL isStdinContainNewlineChar);
void ShowError();
