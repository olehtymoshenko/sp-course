#define _CRT_SECURE_NO_WARNINGS
#include "Main.h"

int main()
{
	char choice = 0;

	for (;;)
	{
		PrintMenu();
		cin >> choice;
		// choose option
		switch (choice)
		{
			case 'a': // print list subkeys by key name
			{
				HKEY hKey = { 0 };
				PHKEY phKey = &hKey;
				if (OpenKey(&phKey, KEY_READ, NULL) == true)
				{
					PrintListSubkeysByKey(hKey);
				}
			}break;
			case 'b': // print all key parameters and their types
			{
				HKEY hKey = { 0 };
				PHKEY phKey = &hKey;
				if (OpenKey(&phKey, KEY_QUERY_VALUE, NULL) == true)
				{
					PrintListParamsByKey(hKey);
				}
			} break;
			case 'c': // find string in reg
			{
				HKEY hKey = { 0 };
				PHKEY phKey = &hKey;
				CHAR fullPath[MAX_PATH];
				if (OpenKey(&phKey, KEY_ALL_ACCESS, fullPath) == true)
				{
					CHAR reqString[MAX_PATH] = {'\0'};
					cout << "Input string for searching:";
					ReadStringWithWhitespaces(reqString, MAX_PATH, false);
					FindStringInReg(hKey, reqString, fullPath);
				}
			} break;
			case 'd': // save key in file
			{
				HANDLE hToken; // access token

				// get access token for getting SE_BACKUP_NAME privige
				if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
				{
					cout << "Cant get access rights (SE_BACKUP_NAME)\n Error code:" << GetLastError() << endl;
				}
				// set privilege for process
				if (SetPrivilege(hToken, SE_BACKUP_NAME, true))
				{
					HKEY hKey = { 0 };
					PHKEY phKey = &hKey;
					// open reg key
					if (OpenKey(&phKey, KEY_ALL_ACCESS, NULL) == true)  
					{
						// save key into file
						SaveKeyIntoFile(hKey);
					}
				}
			} break;
			case 'e': // exit
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

bool FindStringInReg(HKEY hKey, LPCSTR reqStr, LPSTR fullPath)
{
	KEY_INFO keyInfo = { 0 };
	DWORD retCode = ERROR_SUCCESS;
	LPSTR newSubkeyPath;
	if (!GetKeyInfo(hKey, &keyInfo))
	{
		return false;
	}
	// if subkeys exists
	if (keyInfo.cSubKeys)
	{
		// for each subkey
		for (int i = 0; i < keyInfo.cSubKeys; i++)
		{
			keyInfo.cbName = MAX_KEY_LENGTH;
			// get subkey name and size
			retCode = RegEnumKeyEx(hKey,
				i,
				keyInfo.achKey,
				&keyInfo.cbName,
				NULL,
				NULL,
				NULL,
				NULL);
			if (retCode == ERROR_SUCCESS)
			{
				// check for equality with required string
				if (_strcmpi(keyInfo.achKey, reqStr) == 0)
				{
					cout << " * Found in subkey name: " << fullPath << "\\" << keyInfo.achKey << endl;
				}
				// make new full path for subkey (for recursive calling this function)
				newSubkeyPath = (LPSTR)malloc(MAX_VALUE_NAME * sizeof(TCHAR));
				strcpy(newSubkeyPath, fullPath);
				strcat(newSubkeyPath, "\\");
				strcat(newSubkeyPath, keyInfo.achKey);
				HKEY newKey = { 0 };
				// create HKEY for subkey
				if (RegOpenKeyEx(hKey, keyInfo.achKey, 0, KEY_ALL_ACCESS, &newKey) == ERROR_SUCCESS)
				{
					// recursive calling
					FindStringInReg(newKey, reqStr, newSubkeyPath);
				}
				free(newSubkeyPath);
			}
		}
	}
	// try find required string in value name and data
	if (keyInfo.cValues)
	{
		LPSTR lpValue = NULL;
		DWORD dwValue = keyInfo.cchMaxValue + 1;
		
		DWORD dwType = 0;

		LPBYTE lpData = NULL;
		DWORD dwData = 0;

		lpValue = (LPSTR)malloc((keyInfo.cchMaxValue + 1) * sizeof(BYTE));

		for (int i = 0; i < keyInfo.cValues; i++)
		{
			// detect reqired size for data buf and alloc buff
			retCode = RegEnumValueA(hKey, i, lpValue, &dwValue, NULL, NULL, NULL, &dwData);
			lpData = (LPBYTE)malloc((dwData + 1) * sizeof(BYTE));

			// SUPER IMPORTANT, we have to set dwValue on each iteration to max lenght+1 (+1 for zero-character)
			dwValue = keyInfo.cchMaxValue + 1;

			// get data
			retCode = RegEnumValueA(hKey,
				i,
				lpValue,
				&dwValue,
				NULL,
				&dwType,
				lpData,
				&dwData);

			if (retCode == ERROR_SUCCESS)
			{
				if (_strcmpi(lpValue, reqStr) == 0)
				{
					cout << " * Found in value name: " << fullPath << "; " << lpValue << endl;
				}
				if (((dwType & REG_EXPAND_SZ) == REG_EXPAND_SZ) || ((dwType & REG_SZ) == REG_SZ))
				{
					if (_strcmpi((LPSTR)lpData, reqStr) == 0)
					{
						cout << " * Found in data of value " << fullPath << "; " << lpValue << ";\n   data:" << lpData << endl;
					}
				}
			}
		}
	}
	RegCloseKey(hKey);
}

BOOL SetPrivilege(
	HANDLE hToken,          // access token handle
	LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
	BOOL bEnablePrivilege   // to enable or disable privilege
)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(
		NULL,            // lookup privilege on local system
		lpszPrivilege,   // privilege to lookup 
		&luid))        // receives LUID of privilege
	{
		printf("LookupPrivilegeValue error: %u\n", GetLastError());
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.

	if (!AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL))
	{
		printf("AdjustTokenPrivileges error: %u\n", GetLastError());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		printf("You account doesnt have SE_BACKUP_NAME privilege \n");
		return FALSE;
	}
	return TRUE;
}

bool SaveKeyIntoFile(HKEY hKey)
{
	CHAR filePath[MAX_PATH];
	DWORD retCode = ERROR_SUCCESS;
	cout << "Input path to new file:\n";
	// get file path
	ReadStringWithWhitespaces(filePath, MAX_PATH, false);

	// save key in the given file
	retCode = RegSaveKey(hKey, filePath, NULL);
	switch (retCode)
	{
	case ERROR_SUCCESS: 
	{
		cout << "Key saved in the file:\n" << filePath << endl;
		RegCloseKey(hKey);
		return true;
	} break;
	case ERROR_ALREADY_EXISTS:
	{
		cout << "Error! File already exists!\n Entered file path:\n" << filePath << endl;
	} break;
	default:
		cout << "Error! Cant save key into the file\n Error code:" << retCode << endl;
	}
	RegCloseKey(hKey);
	return false;
}

void PrintListParamsByKey(HKEY key)
{
	DWORD i, retCode = ERROR_SUCCESS;
	KEY_INFO keyInfo = { 0 };
	
	DWORD dwType = 0;
	
	LPBYTE lpData = NULL;
	DWORD dwData = 0;
	
	LPSTR lpValue = NULL;
	DWORD dwValue = 0;
	
	GetKeyInfo(key, &keyInfo);

	if (keyInfo.cValues)
	{
		cout << "\t Values count:" << keyInfo.cValues << endl;
		lpValue = (LPSTR)malloc((keyInfo.cchMaxValue + 1) * sizeof(BYTE));
		dwValue = keyInfo.cchMaxValue + 1;

		for (int i = 0; i < keyInfo.cValues; i++)
		{
			// detect reqired size for data buf and alloc buff
			retCode = RegEnumValueA(key, i, lpValue, &dwValue, NULL, NULL, NULL, &dwData);
			lpData = (LPBYTE)malloc((dwData + 1) * sizeof(BYTE));

			// SUPER IMPORTANT, we have to set dwValue on each iteration to max lenght+1 (+1 for zero-character)
			dwValue = keyInfo.cchMaxValue + 1;

			// get data
			retCode = RegEnumValueA(key,
				i,
				lpValue,
				&dwValue,
				NULL,
				&dwType,
				lpData,
				&dwData);

			// decode value type
			if (retCode == ERROR_SUCCESS)
			{
				// check for default value
				if (strcmp(lpValue, "") == 0)
				{
					printf("\n(%d) Value name: %s\n", i + 1, "Default value");
				}
				else
				{
					printf("\n(%d) Value name: %s\n", i + 1, lpValue);
				}

				switch (dwType)
				{
					case REG_BINARY:
					{
						printf("    Value type: REG_BINARY\n    Value data: binary\n");
					} break;
					case REG_DWORD:
					{
						DWORD data = *(DWORD*)(lpData);
						printf("    Value type: REG_DWORD\n    Value data: %#x|%u\n", data, data);
					} break;
					case REG_EXPAND_SZ:
					{
						printf("    Value type: REG_EXPAND_SZ\n    Value data: %s\n", lpData);
					} break;
					case REG_LINK:
					{
						wprintf(L"    Value type: REG_LINK\n    Value data: %ws\n", lpData);
					} break;
					case REG_SZ:
					{
						printf("    Value type: REG_SZ\n    Value data: %s\n", lpData);
					} break;
					case REG_NONE:
					{
						printf("    Value type: REG_NONE\n    Value data: %x\n", *(DWORD*)(lpData));
					} break;
					default:
						printf("    Value type: unknown\n    Value data: %x\n", *(DWORD*)(lpData));
						break;
				}
			}
			free(lpData);
		}
		free(lpValue);
	}
	RegCloseKey(key);
}

void PrintListSubkeysByKey(HKEY key)
{
	DWORD i, retCode;
	KEY_INFO keyInfo = {0};

	// get KEY_INFO
	GetKeyInfo(key, &keyInfo);

	// check for subkeys existing
	if (keyInfo.cSubKeys)
	{
		cout << "\t Subkeys count:" << keyInfo.cSubKeys << endl;
		for (int i = 0; i < keyInfo.cSubKeys; i++)
		{
			keyInfo.cbName = MAX_KEY_LENGTH; // have to for each iteration set this variable
			// get subkey name
			retCode = RegEnumKeyEx(key,
				i,
				keyInfo.achKey,
				&keyInfo.cbName,
				NULL,
				NULL,
				NULL,
				NULL);
			if (retCode == ERROR_SUCCESS) 
			{
				printf("(%d) %s\n", i + 1, keyInfo.achKey);
			}
		}
	}
	RegCloseKey(key);
}

void ReadStringWithWhitespaces(CHAR sBuffNewPath[], DWORD maxBuffSize, BOOL isUsedBeforeInputChar)
{
	memset(sBuffNewPath, '\0', sizeof(sBuffNewPath));
	if (isUsedBeforeInputChar)
		getchar(); // skip first \n (from prev line)
	fgets(sBuffNewPath, maxBuffSize, stdin);
	// is \n was read, than delete it
	if ((strlen(sBuffNewPath) > 0) && (sBuffNewPath[strlen(sBuffNewPath) - 1] == '\n')) 
		sBuffNewPath[strlen(sBuffNewPath) - 1] = '\0';
}

bool GetKeyInfo(HKEY key, KEY_INFO * keyInfo)
{
	// initialize KEY_INFO
  	DWORD retCode = RegQueryInfoKey(key,
									(*keyInfo).achClass,
									&(*keyInfo).cchClassName,
									NULL,
									&(*keyInfo).cSubKeys,
									&(*keyInfo).cbMaxSubKey,
									&(*keyInfo).cchMaxClass,
									&(*keyInfo).cValues,
									&(*keyInfo).cchMaxValue,
									&(*keyInfo).cbMaxValueData,
									&(*keyInfo).cbSecurityDescriptor,
									&(*keyInfo).ftLastWriteTime);
	if (retCode == ERROR_SUCCESS) return true;
	else return false;
}

bool OpenKey(HKEY** hKey, DWORD dwOpenAccess, LPSTR fullPath)
{
	HKEY predKey;
	// make fullPath to hKey, if fullPath is not NULL
	if(fullPath != NULL)  memset(fullPath, '\0', sizeof(fullPath)); 
	int choice = 0;
	cout << "Predefined keys:\n";
	cout << "1 - HKEY_CLASSES_ROOT\n";
	cout << "2 - HKEY_CURRENT_USER\n";
	cout << "3 - HKEY_LOCAL_MACHINE\n";
	cout << "4 - HKEY_USERS\n";
	cout << "5 - HKEY_CURRENT_CONFIG\n";
	cout << "6 - HKEY_PERFORMANCE_DATA\n";
	cout << "Choose predefined key:";
	scanf("%d", &choice);
	switch (choice) // get predefine key
	{
		case 1: 
		{
			predKey = HKEY_CLASSES_ROOT; 
			// make fullPath to hKey, if fullPath is not NULL
			if(fullPath != NULL) strcpy(fullPath, "HKEY_CLASSES_ROOT\\");
		} break;
		case 2: 
		{
			predKey = HKEY_CURRENT_USER; 
			if(fullPath != NULL) strcpy(fullPath, "HKEY_CURRENT_USER\\");
		} break;
		case 3:
		{
			predKey = HKEY_LOCAL_MACHINE; 
			if(fullPath != NULL) strcpy(fullPath, "HKEY_LOCAL_MACHINE\\");
		} break;
		case 4:
		{
			predKey = HKEY_USERS; 
			if(fullPath != NULL) strcpy(fullPath, "HKEY_USERS\\");
		} break;
		case 5:
		{
			predKey = HKEY_CURRENT_CONFIG; 
			if(fullPath != NULL) strcpy(fullPath, "HKEY_CURRENT_CONFIG\\");
		} break;
		case 6:
		{
			predKey = HKEY_PERFORMANCE_DATA; 
			if(fullPath != NULL) strcpy(fullPath, "HKEY_PERFORMANCE_DATA\\");
		} break;
		default:
			return false;
	}

	CHAR keyArr[MAX_KEY_LENGTH] = { '\0' };
	LPSTR key = keyArr;
	cout << "Input subkey(path to subkey) in the given predefined key:\n";
	ReadStringWithWhitespaces(key, MAX_KEY_LENGTH, true);
	// get HKEY for the given path
	if (RegOpenKeyEx(predKey, (LPCSTR)key, 0, dwOpenAccess, *hKey) == ERROR_SUCCESS)
	{
		if (fullPath != NULL) strcat(fullPath, key); // make fullPath to hKey, if fullPath is not NULL
		return true;
	}
	return false;
}

void PrintMenu()
{
	cout << "\t\t Menu\n";
	cout << "\t a - Print a list of subkeys by key name\n";
	cout << "\t b - Print a list of keys parameters with their value and type\n";
	cout << "\t c - Searches the registry for a given string in the key names, key values and their types.\n\t\t Base key set user\n";
	cout << "\t d - Save key as a file\n";
	cout << "\t e - Exit\n";
}
