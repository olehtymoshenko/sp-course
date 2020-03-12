#include <iostream>
#include "windows.h"
#include <vector>
#include <stdio.h>

///////////// Declarate preprocessorc instructions and constants //////////////
#define _CRT_SECURE_NO_WARNINGS

#define TEXT_FIELD_IN_USER_RECORD 80
#define HEADER_CREATE 1
#define HEADER_READ 2

CONST WCHAR PATH_TO_FILE_FOR_RECORDS[] = L"RecordsBase.rb";

//////////////////////////////// Declarete structures /////////////////////////

// Represent one user record in file
typedef struct
{
	WORD wIdRecord; // record id(number)
	FILETIME FLTMCreationTime; // record creation time 
	CHAR cText[TEXT_FIELD_IN_USER_RECORD + 1]; // record text
	WORD wChangeRecordCounter; // record changing count
} USER_RECORD, * PUSER_RECORD;

// Represent file header
typedef struct
{
	WORD wNotNullRecordsCount;
	WORD wFileForRecordsSize;
} FILE_FOR_RECORDS_HEADER, * PFILE_FOR_RECORDS_HEADER;

///////////////////////////// Declarate functions prototype ///////////////////
void ShowMenu();
bool PrintRecordsFile(HANDLE hRecordsFile, DWORD dwCountRecords);
bool InitializeProgrammData(HANDLE* hRecordsFile, DWORD* fileSize, DWORD* countRecords);
bool CreateRecordAndWriteInFile(HANDLE hRecordsFile, DWORD* fileSize, CONST CHAR* recText, DWORD* dwRecordId);
int GetCountRecordsInFile(DWORD fileSize);
bool WriteRecordToFile(HANDLE hRecordsFile, USER_RECORD usRec, DWORD offset);
bool WriteOrGetHeader(HANDLE hRecordsFile, DWORD dwCreateOrGetHeader, PFILE_FOR_RECORDS_HEADER pHeader, BOOL clearFile);
bool DeleteRecord(HANDLE hRecordsFile, DWORD idToDelete, DWORD* countRecords, DWORD* fileSize);
bool ModifyRecord(HANDLE hRecordsFile, DWORD idToModify);