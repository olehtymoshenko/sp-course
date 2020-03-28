#include "Main.h"

/////////////////////////// Functions implementation //////////////////////////
using namespace std;

int main(int argc, char* argv[])
{
	CHAR cUserChoice = '0';
	DWORD dwCountRecords = 0, dwFileSize = 0;
	HANDLE hRecordsFile = NULL;
	if (!InitializeProgrammData(&hRecordsFile, &dwFileSize, &dwCountRecords))
	{
		cout << "Cant initialize file with records\n";
		return 1;
	}

	while (1)
	{
		ShowMenu();
		cin >> cUserChoice;
		switch (cUserChoice)
		{
		case '1': // Create record
		{
			CHAR inBuff[TEXT_FIELD_IN_USER_RECORD] = { 0 };
			cout << "Input text for new record (<= 80 characters)\n or input '0' to create null record\n";
			cin >> inBuff;
			if (!CreateRecordAndWriteInFile(hRecordsFile, &dwFileSize, inBuff, &dwCountRecords))
			{
				cout << "Error. Cant add new record" << endl;
			}
		} break;

		case '2': // Modify record
		{
			DWORD idToMod;
			cout << "Input record id for modify:";
			cin >> idToMod;
			if (idToMod >= dwCountRecords)
			{
				cout << "Error. Not existing record with inputed id\n";
				continue;
			}
			else
			{
				if (!ModifyRecord(hRecordsFile, idToMod))
				{
					cout << "Error. Cant modify record";
					continue;
				}
			}

		} break;

		case '3': // Delete record
		{
			DWORD idToDelete;
			cout << "Input record id for deleting:";
			cin >> idToDelete;
			if (idToDelete >= dwCountRecords)
			{
				cout << "Error. Not existing record with inputed id\n";
				continue;
			}
			else
			{
				if(!DeleteRecord(hRecordsFile, idToDelete, &dwCountRecords, &dwFileSize))
				{
					cout << "Error. Cant delete record\n";
				}
				continue;
			}
		} break;

		case '4': // Print header and all records
		{
			if (!PrintRecordsFile(hRecordsFile, dwCountRecords))
			{
				cout << "Error. Cant print header and all records.\n";
				continue;
			}
		} break;

		case '5': // exit
		{
			CloseHandle(hRecordsFile);
			return 0;
		} break;

		default: cout << "Wrong choice. Try again" << endl;
			continue;
		}
	}
}

// dwCreateOrGetHeader:
// 1 (HEADER_CREATE) - Create
// 2 (HEADER_READ) - Read
bool WriteOrGetHeader(HANDLE hRecordsFile, DWORD dwCreateOrGetHeader, PFILE_FOR_RECORDS_HEADER pHeader, BOOL clearFile)
{
	DWORD cBytes = 0;
	if (SetFilePointer(hRecordsFile, 0, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		return false;
	}
	if (dwCreateOrGetHeader == 1) // Create
	{
		if ((WriteFile(hRecordsFile, pHeader, sizeof(FILE_FOR_RECORDS_HEADER), &cBytes, NULL) == false) ||
			(cBytes != sizeof(FILE_FOR_RECORDS_HEADER)))
		{
			return false;
		}
		if (clearFile)
		{
			if (SetEndOfFile(hRecordsFile) == false)
			{
				return false;
			}
		}
	}
	else if (dwCreateOrGetHeader == 2) // Read
	{
		if ((ReadFile(hRecordsFile, pHeader, sizeof(FILE_FOR_RECORDS_HEADER), &cBytes, NULL) == false) ||
			(cBytes != sizeof(FILE_FOR_RECORDS_HEADER)))
		{
			return false;
		}
	}
	else // wrong flag
	{
		return false;
	}
	return true;
}

bool ModifyRecord(HANDLE hRecordsFile, DWORD idToModify)
{
	FILE_FOR_RECORDS_HEADER header;
	CHAR inBuff[TEXT_FIELD_IN_USER_RECORD + 1] = { 0 };
	
	// Read header
	if (!WriteOrGetHeader(hRecordsFile, HEADER_READ, &header, FALSE))
	{
		return false;
	}

	// get new text
	cout << "Input new text for the record or '0' to reset the record\n";
	cin >> inBuff;
	inBuff[80] = '\0';

	// read record for modify
	USER_RECORD recBuff = { 0 };
	DWORD cBytes=0;
	BOOL isNullRecord = false;
	if (SetFilePointer(hRecordsFile, sizeof(FILE_FOR_RECORDS_HEADER) + sizeof(USER_RECORD) * idToModify,
		NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		return false;
	}
	if ((ReadFile(hRecordsFile, &recBuff, sizeof(USER_RECORD), &cBytes, NULL) == false) ||
		(cBytes != sizeof(USER_RECORD)))
	{
		return false;
	}
	isNullRecord = recBuff.cText[0] == '\0' ? true : false;

	// modify record
	if (strcmp(inBuff, "0") == 0)
	{
		memset(recBuff.cText, '\0', sizeof(recBuff.cText));
	}
	else
	{
		strcpy_s(recBuff.cText, TEXT_FIELD_IN_USER_RECORD, inBuff);
		recBuff.cText[80] = '\0';
	}
	recBuff.wChangeRecordCounter++;
	
	// write record to file
	if (!WriteRecordToFile(hRecordsFile, recBuff, (sizeof(FILE_FOR_RECORDS_HEADER) + sizeof(USER_RECORD) * idToModify)))
	{
		return false;
	}

	// update header
			// was null, will be not null
	if (isNullRecord && (strcmp(inBuff, "0") != 0)) header.wNotNullRecordsCount++;
			// was not null, will be null
	if (!isNullRecord && (strcmp(inBuff, "0") == 0)) header.wNotNullRecordsCount--;

	if (!WriteOrGetHeader(hRecordsFile, HEADER_CREATE, &header, FALSE))
	{
		return false;
	}
	return true;
}

bool DeleteRecord(HANDLE hRecordsFile, DWORD idToDelete, DWORD* countRecords, DWORD* fileSize)
{
	// get file header
	FILE_FOR_RECORDS_HEADER header;
	if (!WriteOrGetHeader(hRecordsFile, HEADER_READ, &header, FALSE))
	{
		return false;
	}

	// set pointer to start of record for deleting
	if (SetFilePointer(hRecordsFile, (sizeof(FILE_FOR_RECORDS_HEADER) + ((idToDelete) * sizeof(USER_RECORD))),
		NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		return false;
	}

	// Read record for deleting
	USER_RECORD delRec;
	if (ReadFile(hRecordsFile, &delRec, sizeof(delRec), 0, NULL) == false)
	{
		return false;
	}

	// get count elements after record for deleting
	WORD countElemToEndFromDeletedEl = (*countRecords) - idToDelete - 1;
	// alloc memory for records after record for deleting
	USER_RECORD* recordsBuff = (USER_RECORD*)calloc(countElemToEndFromDeletedEl, sizeof(USER_RECORD));
	DWORD cBytes;

	// read records after record for deleting
	if ((ReadFile(hRecordsFile, recordsBuff, sizeof(USER_RECORD) * countElemToEndFromDeletedEl, &cBytes, NULL) == false) ||
		cBytes != sizeof(USER_RECORD) * countElemToEndFromDeletedEl)
	{
		return false;
	}

	// set new id to records after deleted record
	for (WORD i = 0; i < countElemToEndFromDeletedEl; i++)
	{
		recordsBuff[i].wIdRecord = i + idToDelete;
	}

	// set pointer on start record for deleting
	if (SetFilePointer(hRecordsFile, 0 - (sizeof(USER_RECORD) * countElemToEndFromDeletedEl + sizeof(USER_RECORD)),
		NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
	{
		return false;
	}

	// write records after deleted record to file
	if ((WriteFile(hRecordsFile, recordsBuff, sizeof(USER_RECORD) * countElemToEndFromDeletedEl, &cBytes, NULL) == false) ||
		(cBytes != sizeof(USER_RECORD) * countElemToEndFromDeletedEl))
	{
		return false;
	}
	free(recordsBuff);
	SetEndOfFile(hRecordsFile); // after writing set new end of file (without 1 record)

	// update header and programm variables
	header.wNotNullRecordsCount = delRec.cText[0] == '\0' ? 
		header.wNotNullRecordsCount : header.wNotNullRecordsCount - 1;
	header.wFileForRecordsSize = header.wFileForRecordsSize - sizeof(USER_RECORD);
	(*countRecords)--;
	(*fileSize) -= sizeof(USER_RECORD);

	// write new header to file
	if (!WriteOrGetHeader(hRecordsFile, HEADER_CREATE, &header, FALSE))
	{
		return false;
	}
	return true;
}

bool CreateRecordAndWriteInFile(HANDLE hRecordsFile, DWORD * fileSize, CONST CHAR* recText, DWORD * dwRecordId)
{
	// init record to write
	BOOL isNullRecord = false;
	USER_RECORD usRec;
	if (strcmp(recText, "0")==0)
	{
		memset(usRec.cText, '\0', sizeof(usRec.cText));
		isNullRecord = true;
	}
	else
	{
		strcpy_s(usRec.cText, TEXT_FIELD_IN_USER_RECORD, recText);
		usRec.cText[80] = '\0';
		isNullRecord = false;
	}
	usRec.wChangeRecordCounter = 0;
	usRec.wIdRecord = *dwRecordId;

	SYSTEMTIME sysTime = { 0 }; 
	FILETIME fileTime = { 0 };
	GetLocalTime(&sysTime);
	SystemTimeToFileTime(&sysTime, &fileTime);
	usRec.FLTMCreationTime = fileTime;
	
	// write record and update header
	if (!WriteRecordToFile(hRecordsFile, usRec, sizeof(FILE_FOR_RECORDS_HEADER) + (*dwRecordId) * sizeof(USER_RECORD)))
	{
		return false;
	}

	// Read header
	FILE_FOR_RECORDS_HEADER header = { 0 };
	WriteOrGetHeader(hRecordsFile, HEADER_READ, &header, FALSE);
	header.wFileForRecordsSize += sizeof(USER_RECORD);
	header.wNotNullRecordsCount = isNullRecord ? header.wNotNullRecordsCount : ++header.wNotNullRecordsCount;
	WriteOrGetHeader(hRecordsFile, HEADER_CREATE, &header, FALSE);

	// update programm variables about file with records
	(*dwRecordId)++;
	(*fileSize) += sizeof(USER_RECORD);

	return true;
}

bool WriteRecordToFile(HANDLE hRecordsFile, USER_RECORD usRec, DWORD offset)
{
	if (SetFilePointer(hRecordsFile, offset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		return false;
	}

	DWORD countWritenBytes = 0;
	if ((WriteFile(hRecordsFile, &usRec, sizeof(usRec), &countWritenBytes, NULL) == false) ||
		(countWritenBytes != sizeof(usRec)))
	{
		return false;
	}
	return true;
}

void ShowMenu()
{
	cout << "\t *** MENU ***" << endl;
	cout << " 1 - Create record" << endl;
	cout << " 2 - Modify record" << endl;
	cout << " 3 - Delete record" << endl;
	cout << " 4 - Print header and all records" << endl;
	cout << " 5 - Exit" << endl;
}

int GetCountRecordsInFile(DWORD fileSize)
{
	WORD recordsCount = (fileSize - sizeof(FILE_FOR_RECORDS_HEADER)) / sizeof(USER_RECORD);
	return recordsCount;
}

bool InitializeProgrammData(HANDLE* hRecordsFile, DWORD* fileSize, DWORD* countRecords)
{
	// open file
	*hRecordsFile = CreateFile(PATH_TO_FILE_FOR_RECORDS,
		GENERIC_ALL,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (*hRecordsFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	// get file size
	*fileSize = GetFileSize(*hRecordsFile, NULL);
	if (*fileSize == INVALID_FILE_SIZE)
	{
		return false;
	}

	if (*fileSize <= sizeof(FILE_FOR_RECORDS_HEADER))
	{
		FILE_FOR_RECORDS_HEADER header;
		header.wNotNullRecordsCount = 0;
		header.wFileForRecordsSize = sizeof(FILE_FOR_RECORDS_HEADER);
		if (!WriteOrGetHeader(*hRecordsFile, HEADER_CREATE, &header, TRUE))
		{
			return false;
		}
		*fileSize = sizeof(FILE_FOR_RECORDS_HEADER);
	}

	// get count records in file
	*countRecords = GetCountRecordsInFile(*fileSize);
	return true;
}

bool PrintRecordsFile(HANDLE hRecordsFile, DWORD dwCountRecords)
{
	FILE_FOR_RECORDS_HEADER header = { 0 };
	USER_RECORD recordsBuff = { 0 };
	DWORD cReadedBytes = 0;
	if (!WriteOrGetHeader(hRecordsFile, HEADER_READ, &header, FALSE))
	{
		return false;
	}
	cout << "********* HEADER *********\n";
	cout << "* Not null records count:" << header.wNotNullRecordsCount << endl;
	cout << "* File size:" << header.wFileForRecordsSize <<" Bytes" << endl;

	while ((ReadFile(hRecordsFile, &recordsBuff, sizeof(USER_RECORD), &cReadedBytes, NULL) != false) &&
		(cReadedBytes == sizeof(USER_RECORD)))
	{
		SYSTEMTIME sysTime;
		FileTimeToSystemTime(&recordsBuff.FLTMCreationTime, &sysTime);
		cout << "********* RECORD *********\n";
		cout << "* ID:" << recordsBuff.wIdRecord << endl;
		printf("* Creation time:%d %d %d, %d %d\n", sysTime.wDay, sysTime.wMonth, sysTime.wYear, sysTime.wHour, sysTime.wMinute);
		cout << "* Changes count:" << recordsBuff.wChangeRecordCounter << endl;
		cout << "* Text:" << recordsBuff.cText << endl;
	}
	return true;
}