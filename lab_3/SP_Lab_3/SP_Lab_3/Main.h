#include <stdio.h>
#include "windows.h"
#include "iostream"
#include "tchar.h"
#include "processthreadsapi.h"


#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

using namespace std;

// struct for key information (mostly use in RegQueryInfoKey)
typedef struct {
	TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
	DWORD    cbName;                   // size of name string 
	TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
	DWORD    cchClassName = MAX_PATH;  // size of class string 
	DWORD    cSubKeys = 0;               // number of subkeys 
	DWORD    cbMaxSubKey;              // longest subkey size 
	DWORD    cchMaxClass;              // longest class string 
	DWORD    cValues;              // number of values for key 
	DWORD    cchMaxValue;          // longest value name 
	DWORD    cbMaxValueData;       // longest value data 
	DWORD    cbSecurityDescriptor; // size of security descriptor 
	FILETIME ftLastWriteTime;      // last write time 
} KEY_INFO, * pKEY_INFO;

// common functions
// print menu to console
void PrintMenu();

// open key in registry, fullPath can be NULL
bool OpenKey(HKEY** hKey, DWORD dwOpenAccess, LPSTR fullPath);

// Read string form sdtin 
void ReadStringWithWhitespaces(CHAR sBuffNewPath[], DWORD maxBuffSize, BOOL isUsedBeforeInputChar);

// Get key information (KEY_INFO struct)
bool GetKeyInfo(HKEY key, KEY_INFO* keyInfo);

//--------------------------------------------//
// function for print list subkeys by key name 
void PrintListSubkeysByKey(HKEY key);

//--------------------------------------------//
// function for print all key parameters and their types 
void PrintListParamsByKey(HKEY key);

//--------------------------------------------//
// function for search string in reg (recursive function), output in 
// stdout all hits
bool FindStringInReg(HKEY hKey, LPCSTR reqStr, LPSTR fullPath);

//--------------------------------------------//
// function for save key in file
// save key into the file
bool SaveKeyIntoFile(HKEY hKey);
// set requierd privilege (SE_BACKUP_NAME) for current process
BOOL SetPrivilege(
	HANDLE hToken,          // access token handle
	LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
	BOOL bEnablePrivilege   // to enable or disable privilege
);