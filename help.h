#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

typedef struct strings{
	char** strings;
	int size;
    int error;
}Strings;


int stringToPositiveFloat(char* string, float *number);

void freeStrings(Strings strings);

Strings split(char* str , char regex);

char* appendStrings(int count, ...);

int stringToFloat(char* string, float *number);

int stringToInt(char* string, int *number);

int onlyNumbers(char* string);

int onlyLetters(char* string);

int emailPattern(char* string);

int onlyLettersAndNumbers(char* string);
int isIp(char* string);