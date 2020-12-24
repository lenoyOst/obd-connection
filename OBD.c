#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <stdio.h>

char** scanSerial(void)
{
	int current = 0;
	char** ports = (char**)malloc(10*sizeof(char*));
#ifdef _WIN32 
	HANDLE hCom;
	OVERLAPPED o;
	BOOL fSuccess;
	DWORD dwEvtMask;
	char text[11] = "\\\\.\\COM";
	//going through all the possible ports (com 1-256)
	for (int i = 0; i < 256; i++)
	{
		if (i < 10) { text[7] = i + 49; }

		else if (i < 100) {
			text[7] = (i / 10) + 48;
			text[8] = (i % 10) + 48;
		}
		else
		{

			text[7] = (i / 100) + 48;
			text[8] = (i / 10) % 10 + 48;
			text[9] = (i % 10) + 48;
		}
		hCom = CreateFile(TEXT(text),
			GENERIC_READ | GENERIC_WRITE,
			0,    // exclusive access 
			NULL, // default security attributes 
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED,
			NULL
		);
		if (hCom == INVALID_HANDLE_VALUE)
		{
			// Handle the error. 
			//printf("CreateFile failed with error %d.\n", GetLastError());
		}
		else
		{
			printf("%s is active\n", text);
			ports[current] = malloc(sizeof(text)+1);
			ports[current] = _strdup(text);
			current++;
		}
		if (current == 9)
		{
			break;
			//toDo : warning , error
		}
	}
#elif __linux__
	//toDo

#endif
	return ports;
}

