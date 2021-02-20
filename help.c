#include "help.h"

// --------------------------------
// convert string to positive float
// --------------------------------
int stringToPositiveFloat(char* string, float *number)
{
    char c;
    int i = 0;
    int len = strlen(string);
    *number = 0;
    int place = 10;
    
    // convert string before floating point to integer
    while(i < len || (c = string[i]) != '.')
    {
        if(c <'0' || c>'9')
        {
            return -1;
        }

        *number += c-'0';
        *number *= 10;

        i++;
    }
    *number/=10;

    if(i== len)
    {
        return 1;
    }
    i++;

    // convert string after floating point to float
    while(i < len)
    {
        c = string[i];

        if(c <'0' || c>'9')
        {
            return -1;
        }

        *number += (c-'0')/(place);
        place *= 10;

        i++;
    }

    return 1;
}

// ----------------------
// split string by spaces
// ----------------------
Strings split(char* str)
{
    Strings stringList;
	int i = 0;
    int start = 0;
    int end = 0;
	int count = 0;
    stringList.size = 0;

    while(str[i] != '\0')
    {
        if(str[i] == ' ')
           stringList.size++;
        i++;
    }
    stringList.size++;
	stringList.strings = (char**)malloc(sizeof(char*) * stringList.size);
    if(stringList.strings == NULL)
    {
        stringList.size = 0;
        stringList.error = 1;
        return stringList;
    }
	while(str[end] != '\0')
	{
        if(str[end] == ' ')
        {
            stringList.strings[count] = (char*)malloc(sizeof(char) * (end - start + 1));
            if(stringList.strings[count] == NULL)
            {
                for(i = 0; i < count; i++)
                {
                    free(stringList.strings[i]);
                }
                stringList.size = 0;
                stringList.error = 1;
                return stringList;
            }
            memcpy(stringList.strings[count], str + start, end - start);
            stringList.strings[count][end - start] = '\0';
            count++;
            start = end + 1;
        }
        end++;
	}
    stringList.strings[count] = (char*)malloc(sizeof(char) * (end - start + 1));
    if(stringList.strings[count] == NULL)
    {
       for(i = 0; i < count; i++)
       {
         free(stringList.strings[i]);
       }
        stringList.size = 0;
        stringList.error = 1;
        return stringList;
    }
    memcpy(stringList.strings[count], str + start, end - start);
    stringList.strings[count][end - start] = '\0';
    count++;
    start = end + 1;
    stringList.error = 0;
    return stringList;
}

// ------------------
// free String struct
// ------------------
void freeStrings(Strings strings)
{   
    int i;
    for(i = 0; i < strings.size; i++)
    {
        free(strings.strings[i]);
    }
}

// ------------------
// free String struct
// ------------------
char* appendStrings(int count, ...)
{
    va_list list; 
    char* str;
    char* newStr;
    int i;
    int totalSize = 0;

    va_start(list, count);
    for(i = 0; i < count; i++)
    {
        str = va_arg(list, char *);
        totalSize += strlen(str);
    }
    va_end(list);

    newStr = (char*)malloc(sizeof(char)* (totalSize + 1));
    if(newStr == NULL)
    {
        return NULL;
    }

    va_start(list, count);
    newStr[0] = '\0';
    for(i = 0; i < count; i++)
    {
        str = va_arg(list, char *);
        strcat(newStr, str);
    }
    va_end(list);
    return newStr;
}

// -----------------------
// convert string to float
// -----------------------
int stringToFloat(char* string, float *number)
{
    int len = strlen(string);
    int i = 0;
    int sign = 1;
    int place = 10;
    
    *number = 0;

    if(len == 0)
        return -1;
    if(string[i] =='-')
    {
        sign = -1;
        i++;
        if(len == 1)
            return -1;
    }

    while(i < len || string[i] != '.')
    {
        if(string[i] <'0' || string[i]>'9')
            return -1;

        *number += string[i]-'0';
        *number *= 10;

        i++;
    }
    *number/=10;

    if(i== len)
        return 1;

    i++;
    while(i < len)
    {
        if(string[i] <'0' || string[i]>'9')
            return -1;

        *number += (string[i]-'0')/(place);
        place *= 10;

        i++;
    }
    *number *= sign;
    return 1;
}

// ---------------------
// convert string to int
// ---------------------
int stringToInt(char* string, int *number)
{
    int len = strlen(string);
    int i = 0;
    int sign = 1;
    
    *number = 0;

    if(len == 0)
        return -1;
    if(string[i] =='-')
    {
        sign = -1;
        i++;
        if(len == 1)
            return -1;
    }

    while(i<len)
    {
        if(string[i]<'0' || string[i]>'9')
            return -1;

        *number += string[i]-'0';
        *number *= 10;
        i++;
    }
    *number/=10;

    *number *= sign;

    return 1;
}

// ------------------------------------
// check if string includes only numbers
// ------------------------------------
int onlyNumbers(char* string)
{
    int len = strlen(string);
    int i = 0;

    if(len == 0)
        return 0;
  
  while(i<len)
    {
        if(string[i]<'0' || string[i]>'9')
            return 0;

        i++;
    }
   
   return 1;
}

// ------------------------------------
// check if string includes only letters
// ------------------------------------
int onlyLetters(char* string)
{
    int len = strlen(string);
    int i = 0;
   
    if(len == 0)
        return 0;
   
    while(i<len)
    {
        if(!((string[i]>='a' && string[i]<='z') || (string[i]>='A' && string[i]<='Z')))
            return 0;

        i++;
    }
   
    return 1;
}

// ----------------------------------------------------------
// check if string includes only letters, numbers, '.' and '@'
// ----------------------------------------------------------
int emailPattern(char* string)
{
    int len = strlen(string);
    int i = 0;
   
    if(len == 0)
        return 0;
   
   
    while(i<len)
    {
        if(!((string[i]>='a' && string[i]<='z') || (string[i]>='A' && string[i]<='Z') || string[i]=='@' || string[i]=='.' || (string[i]>='0' && string[i]<='9')))
            return 0;

        i++;
    }
    return 1;
}

// ----------------------------------------------
// check if string includes only letters and numbers
// ----------------------------------------------
int onlyLettersAndNumbers(char* string)
{
    int len = strlen(string);
    int i = 0;
   
    if(len == 0)
        return 0;
   
    while(i<len)
    {
        if(!((string[i]>='a' && string[i]<='z') || (string[i]>='A' && string[i]<='Z') || (string[i]>='0' && string[i]<='9')))
            return 0;

        i++;
    }
   
    return 1;
}
//
//if error return 0 , sucsses 1
//
int isIp(char* string)
{
	int len = strlen(string);
    int i = 0;
	int j = 0;
    char number[3];
    number[0]='\0';
    int check;
   
    if(len > 15 || len<7){return 0;}

    while(string[i]!='.' && j < 3 && len != i)
    {
        number[j] = string[i];
        i++;
		j++;
    }
	if(j < 3)
		number[j] = '\0'; 
	
    if(stringToInt(number,&check) < 0 ){return 0;}
    if(check>255 || check<0 || i<1 || string[i]!='.' || i>=len){return 0;}
	
    number[0]='\0';
	j = 0;
	i++;
	
    while(string[i]!='.' && j < 3 && len != i)
    {
        number[j] = string[i];
        i++;
		j++;
    }
	if(j < 3)
		number[j] = '\0'; 
		
    if(stringToInt(number,&check) < 0 ){return 0;}
    if(check>255 || check<0 || i<3 || string[i]!='.' || i>=len){return 0;}
		
    number[0]='\0';
	j = 0;
	i++;
	
    while(string[i]!='.' && j < 3 && len != i)
    {
        number[j] = string[i];
        i++;
		j++;
    }
	if(j < 3)
		number[j] = '\0';
		
    if(stringToInt(number,&check) < 0 ){return 0;}
    if(check>255 || check<0 || i<5 || string[i]!='.' || i>=len) {return 0;}
	
    number[0]='\0';
	j = 0;
	i++;
	
    while(i < len && j < 3)
    {
        number[j] = string[i];
        i++;
		j++;
    }
	if(j < 3)
		number[j] = '\0'; 
		
    if(stringToInt(number,&check) < 0 ){return 0;}
    if(check>255 || check<0 || i < len){return 0;}
    return 1;
}