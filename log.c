#include"log.h"
FILE* fd;
// ------------
// creat LogFile
// ------------
void creatLog()
{
    fd = fopen("log.txt" , "w");
}
// ------------
// write to log
// ------------
void writeToLog(char* msg)
{
    fwrite(msg, sizeof(char), strlen(msg), fd);
	fwrite("\n", sizeof(char), 1, fd);
}
// ------------
// close LogFile
// ------------
void closeLog()
{
    fclose(fd);
}