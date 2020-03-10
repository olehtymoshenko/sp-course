#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "windows.h"
#include <locale.h>
#include <iostream>

using namespace std;

#define ANSI_KEY "-a"
#define UNICODE_KEY "-u"

#define SIZE_BUFF_FOR_READING_FILE 1024

void ANSI_To_Unicode(LPCSTR);
void Unicode_To_ANSI(LPCSTR);
void ShowError();
bool ConcatFileNameWithString(char* FileName, char* StringToConcat, char* resultString, int len_resultString);

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "rus");
    if (argc != 3)
    {
        fprintf(stderr, "Error! Incorrect arguments count for programm\n");
        fprintf(stderr, "Necessary keys to run programm: ""-codepage_key filename_for_recoding\n");
        fprintf(stderr, "Codepage key: '-a' - ANSI input\n");
		fprintf(stderr, "Codepage key: '-u' - Unicode input\n");
        return 0;
    }
    else
    {
        if (strcmp(argv[1], ANSI_KEY) == 0)
        {
            ANSI_To_Unicode(argv[2]);
        }
        else if (strcmp(argv[1], UNICODE_KEY) == 0)
        {
            Unicode_To_ANSI(argv[2]);
        }
        else
        {
            fprintf(stderr, "Incorrect arguments\n");
        }
    }
    fprintf(stdout, " *** Recoding completed successfully *** ");
    return 0;
}

void ANSI_To_Unicode(LPCSTR fileName)
{
    HANDLE handleSourceFile, handleDestinationFile; // handles to files 

    CHAR asciBuff[SIZE_BUFF_FOR_READING_FILE]; // buff to read text from source file
    WCHAR * unicodeBuff; // buff contain encoded ASCI text to Unicode

    DWORD countRdChars, countWrWChars; // counters of read Symbols(in ReadFIle) and writen Symbols(in WriteFile)  
    BOOL resultRdFile = true; 

	char additionalTextToFileName[] = "_encoded_To_Unicode";
	char nameDestinationFile[FILENAME_MAX]; // name to file with recoded text
	ConcatFileNameWithString((char*)fileName, additionalTextToFileName, nameDestinationFile, FILENAME_MAX);
    printf(" *** Recoded file will be located in *** \n %s\n", nameDestinationFile);
    // OPEN FILES
    handleSourceFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ,
                                  NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handleSourceFile == INVALID_HANDLE_VALUE)
    {
        ShowError();
        exit(EXIT_FAILURE);
    }
    handleDestinationFile = CreateFile(nameDestinationFile, GENERIC_WRITE, 0,
                                      NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handleDestinationFile == INVALID_HANDLE_VALUE)
    {
        ShowError();
        exit(EXIT_FAILURE);
    }
    // In cycle read all file in blocks of size - SIZE_BUFF_FOR_READING_FILE
    while (((resultRdFile = ReadFile(handleSourceFile, &asciBuff, sizeof(asciBuff), &countRdChars, NULL)) != false) && (countRdChars != 0))
    {
		// find necessary buff size for recoded text
		int sizeToAllocate = MultiByteToWideChar(CP_UTF8, 0, asciBuff, countRdChars, NULL, 0);

		// allocate necessary buff
		unicodeBuff = (WCHAR *)calloc(sizeToAllocate, sizeof(WCHAR));

        // recode text to Unicode from ASCI
        MultiByteToWideChar(CP_UTF8, 0, asciBuff, countRdChars, unicodeBuff, sizeToAllocate);

        // write recoded block to new file
        if ((!WriteFile(handleDestinationFile, unicodeBuff, sizeToAllocate * sizeof(WCHAR), &countWrWChars, NULL)) ||
			(countWrWChars != sizeToAllocate * sizeof(WCHAR)))
        {
            ShowError();
            exit(EXIT_FAILURE);
        }
		free(unicodeBuff);
    }
    if (resultRdFile == false && countRdChars != 0)
    {
        ShowError();
        exit(EXIT_FAILURE);
    }

    CloseHandle(handleSourceFile);
    CloseHandle(handleDestinationFile);
}

void Unicode_To_ANSI(LPCSTR fileName)
{
    HANDLE handleSourceFile, handleDestinationFile; // handles to files 

    CHAR rawBytesBuff[SIZE_BUFF_FOR_READING_FILE]; // buff to read text from source file
	WCHAR * unicodeBuff; // buff to contain encoded raw bytes to wide char
    CHAR * asciBuff; // buff contain encoded ASCI text to Unicode

    DWORD countReadedBytes, countWritenBytes; // counters of read Symbols(in ReadFIle) and writen Symbols(in WriteFile)  
    BOOL resultRdFile = true;

	CHAR additionalTextToFileName[] = "_encoded_To_ANSI";
	CHAR nameDestinationFile[FILENAME_MAX]; // name to file with recoded text

	ConcatFileNameWithString((char*)fileName, additionalTextToFileName, nameDestinationFile, FILENAME_MAX);
    printf(" *** Recoded file will be located in *** \n %s\n", nameDestinationFile);
    // OPEN FILES
    handleSourceFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ,
								  NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handleSourceFile == INVALID_HANDLE_VALUE)
    {
        ShowError();
        exit(EXIT_FAILURE);
    }

    handleDestinationFile = CreateFile(nameDestinationFile, GENERIC_WRITE, 0,
									   NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handleDestinationFile == INVALID_HANDLE_VALUE)
    {
        ShowError();
        exit(EXIT_FAILURE);
    }
	// read first 2 bytes which mean ending style (BE or LE), because it doesnt need for ansi files
    if (!ReadFile(handleSourceFile, &rawBytesBuff, 2, &countReadedBytes, NULL))
    {
        ShowError();
        exit(EXIT_FAILURE);
    }

    // In cycle read all file in blocks of size - SIZE_BUFF_FOR_READING_FILE
    while (((resultRdFile = ReadFile(handleSourceFile, &rawBytesBuff, sizeof(rawBytesBuff), &countReadedBytes, NULL)) != false) && (countReadedBytes != 0))
    {
		// Read raw bytes, than get count of unicode symbols in this raw bytes
		int unicodeLenght = MultiByteToWideChar(CP_UTF8, 0, rawBytesBuff, countReadedBytes, NULL, 0);

		// Allocate buff for containing raw bytes encoded to wide char
		unicodeBuff = (WCHAR*)calloc(unicodeLenght, sizeof(WCHAR));
		
		// Encode raw bytes to wide char
		MultiByteToWideChar(CP_UTF8, 0, rawBytesBuff, countReadedBytes,
						    unicodeBuff, unicodeLenght);

		// Get necessary count bytes to contain convert wide char array to multibyte array
		int asciLenght = WideCharToMultiByte(CP_ACP, 0, unicodeBuff, unicodeLenght, NULL, 0, NULL, NULL);
        
		// Allocate necessary buff
		asciBuff = (CHAR *) calloc(asciLenght, sizeof(CHAR));

		// convert wide char to multibyte
		WideCharToMultiByte(CP_ACP, 0, unicodeBuff, unicodeLenght, asciBuff,
							asciLenght, NULL, NULL);
		// write recoded block to new file
        if ((!WriteFile(handleDestinationFile, asciBuff, asciLenght, &countWritenBytes, NULL)) || (countWritenBytes != asciLenght))
        {
            ShowError();
            exit(EXIT_FAILURE);
        }
		free(unicodeBuff);
		free(asciBuff);
    }
    if (resultRdFile == false && countReadedBytes != 0)
    {
        ShowError();
        exit(EXIT_FAILURE);
    }

    CloseHandle(handleSourceFile);
    CloseHandle(handleDestinationFile);
}

// This function concat file name (file name with format: .txt, .xml etc) with string,
// this string insert in end of file name, but before format (i.e. before last dot in file name string)
bool ConcatFileNameWithString(char * FileName, char * StringToConcat, char * resultString, int len_resultString)
{	
	for (int i = 0; i < len_resultString; i++)
	{
		resultString[i] = '\0';
	}
	if ((strlen(FileName) + strlen(StringToConcat)) > FILENAME_MAX)
	{
		resultString = NULL;
		return false;
	}
	char * refToLastDot = strrchr(FileName, '.');
	int lastDotPositionInFileName = refToLastDot - FileName;
	strncpy(resultString, FileName, lastDotPositionInFileName);
	snprintf(resultString, FILENAME_MAX, "%s%s%s", resultString, StringToConcat, refToLastDot);

	return true;
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
