#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct connections
{
	char** list;
	int size;
} Connections;

Connections* scanSerial(void);