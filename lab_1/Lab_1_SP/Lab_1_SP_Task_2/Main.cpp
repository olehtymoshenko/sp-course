#include <stdio.h>
#include "windows.h"
#include <locale.h>
#include <iostream>

using namespace std;

#define ANSI_KEY "-a"
#define UNICODE_KEY "-u"

#define SIZE_BUFF_FOR_READING_FILE 2048

void ANSI_To_Unicode(LPCSTR);
void Unicode_To_ANSI(LPCSTR);
void ShowError();


int main(int argc, char* argv[])
{
    setlocale(LC_CTYPE, "rus");
    if (argc != 3)
    {
        fprintf(stderr, "������! �� �������� ��� ��������� ��� ���������\n");
        fprintf(stderr, "����������� �����: ""���_��������� ����_���_��������� ���_�����_���_���������������\n");
        fprintf(stderr, "����� ��� ���������: '-a'  - ����� � ANSI ���������\n");
        fprintf(stderr, "����� ��� ���������: '-u'  - ����� � Unicode ���������\n");
        system("Pause");
        return 0;
    }
    else
    {
        printf("%s", argv[1]);
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
            fprintf(stderr, "������ ����� ����������\n");
        }
    }
    fprintf(stdout, "����������� ���� ������� ���������");
    return 0;
}

void ANSI_To_Unicode(LPCSTR fileName)
{
    HANDLE handleSourceFile, handleDestinationFile; // handles to files 

    CHAR rdBuff[SIZE_BUFF_FOR_READING_FILE]; // buff to read text from source file
    WCHAR buffCharsEncodedToWChar_t[SIZE_BUFF_FOR_READING_FILE]; // buff contain encoded ASCI text to Unicode

    DWORD countReadedChars, countWritenChars; // counters of read Symbols(in ReadFIle) and writen Symbols(in WriteFile)  
    BOOL resultRdFile = true; 

    char nameDestinationFile[256]; // name to file with recoded text
    snprintf(nameDestinationFile, sizeof(nameDestinationFile), "%s%s", "Encoded_To_Unicode_", fileName);
    printf("��������������� ����� ����� ��������� � \n %s\n", nameDestinationFile);
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
    while (((resultRdFile = ReadFile(handleSourceFile, &rdBuff, sizeof(rdBuff), &countReadedChars, NULL)) != 0) && (countReadedChars != 0))
    {
        // recode text to Unicode from ASCI
        MultiByteToWideChar(GetACP(), 0, rdBuff, countReadedChars, buffCharsEncodedToWChar_t, SIZE_BUFF_FOR_READING_FILE);

        // write recoded block to new file
        if (!WriteFile(handleDestinationFile, buffCharsEncodedToWChar_t, countReadedChars*2, &countWritenChars, NULL))
        {
            ShowError();
            exit(EXIT_FAILURE);
        }
    }
    if (resultRdFile == false && countReadedChars != 0)
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

    WCHAR rdBuff[SIZE_BUFF_FOR_READING_FILE]; // buff to read text from source file
    CHAR buffWCHARSEncodedToCHAR[SIZE_BUFF_FOR_READING_FILE]; // buff contain encoded ASCI text to Unicode
    CCHAR unknownSymb(254);
    BOOL isUsedUnknownSymb = false;

    DWORD countReadedBytes, countWritenBytes; // counters of read Symbols(in ReadFIle) and writen Symbols(in WriteFile)  
    BOOL resultRdFile = true;

    char nameDestinationFile[256]; // name to file with recoded text
    snprintf(nameDestinationFile, sizeof(nameDestinationFile), "%s%s", "Encoded_To_ASCI_", fileName);
    printf("��������������� ����� ����� ��������� � \n %s\n", nameDestinationFile);
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
    if (!ReadFile(handleSourceFile, &rdBuff, 2, &countReadedBytes, NULL))
    {
        ShowError();
        exit(EXIT_FAILURE);
    }
    // In cycle read all file in blocks of size - SIZE_BUFF_FOR_READING_FILE
    while (((resultRdFile = ReadFile(handleSourceFile, &rdBuff, sizeof(rdBuff), &countReadedBytes, NULL)) != 0) && (countReadedBytes != 0))
    {
        WideCharToMultiByte(GetACP(), 0, rdBuff, countReadedBytes / sizeof(rdBuff[0]), buffWCHARSEncodedToCHAR,
                             countReadedBytes / sizeof(rdBuff[0]), NULL, NULL);
        // write recoded block to new file
        if (!WriteFile(handleDestinationFile, buffWCHARSEncodedToCHAR, countReadedBytes / sizeof(rdBuff[0]), &countWritenBytes, NULL))
        {
            ShowError();
            exit(EXIT_FAILURE);
        }
    }
    if (resultRdFile == false && countReadedBytes != 0)
    {
        ShowError();
        exit(EXIT_FAILURE);
    }

    CloseHandle(handleSourceFile);
    CloseHandle(handleDestinationFile);
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