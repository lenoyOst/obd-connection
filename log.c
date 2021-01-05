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
// close LogFile
// ------------
void close()
{
    fclose(fd);
}
// ------------
// write to log
// ------------
void writeToLog()
{
    
}