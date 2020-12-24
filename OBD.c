#include "OBD.h"

Connections* scanSerial(void)
{
	Connections* connections;
	int i, j;
	int current = 0;
	char** ports;
	

#ifdef _WIN32 
	HANDLE hCom;
	OVERLAPPED o;
	BOOL fSuccess;
	DWORD dwEvtMask;
	char text[11] = "\\\\.\\COM";

	ports = (char**)malloc(10 * sizeof(char*));
	if (ports == NULL)
		return NULL;

	//going through all the possible ports (com 1-256)
	for (i = 1; i < 10; i++)
	{
		if (i < 10) { text[7] = i + 48; }

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
			//
		}
		else
		{
			ports[current] = (char**)malloc(sizeof(text)+1);
			if (ports[current] == NULL)
			{
				for (j = 0; j < current; j++)
				{
					free(ports[j]);
				}
				free(ports);
				return NULL;
			}
			ports[current] = _strdup(text);
			current++;
		}
		if (current == 10)
		{
			break;
			//toDo : warning , error
		}
	}
#elif __linux__
	//toDo

#endif
	connections = (Connections*)malloc(sizeof(Connections));
	if (connections == NULL)
		return NULL;

	connections->list = ports;
	connections->size = current;

	return connections;
}

