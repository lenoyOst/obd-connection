#ifdef _WIN32 
#include <windows.h>
#include <tchar.h>
#elif __linux__
#include <glob.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "log.h"


#define MAX_POTENTIALS_PORTS 10
#define MAX_MESSEGE_SIZE 1024

// -------
// SUCCESS
// -------

#define SUCCESS 0

// --------
// WARNINGS
// --------

#define ALLOCATION_WARNING 1
#define RICHED_MAX_POTENTIALS_PORTS_WARNING 2

// ------
// EVENTS
// ------

#define NOT_OBD 3

// ------
// ERRORS
// ------

#define ERROR 4

#define ALLOCATION_ERROR 5
#define WRITE_ERROR 7
#define READ_ERROR 8
#define CLOSE_ERROR 9
#define CAR_NOT_CONNECTED_ERROR 10
#define NO_DATA_ERROR 10

typedef struct connections
{
	char** list;
	int size;
} Connections;

typedef struct commands
{
	//at commands
	char* reset;
	char* echo_on;
	char* echo_off;
	char* header_on;
	char* header_off;
	char* describe_protocol;
	char* timing_off;
	char* timing_auto1;
	char* timing_auto2;
	char* linefeeds_off;
	char* linefeeds_on;
	char* set_protocol_auto;

	char* monitor;

	char* pin1;
	char* pin2;
	char* pin3;
	char* pin4;
	char* pin5;
	char* pin6;
	char* pin7;
	char* pin8;
	char* pin9;
	char* pin10;
	char* pin11;
	char* pin12;
	char* pin13;
	char* pin14;
	char* pin15;
	char* pin16;

	char* config_monitor;
	char* long_msg_ON;
	char* protocolJ1939;

	//mode1 commands
	char* rpm;
	char* speed;
	char* throtle;
	char* fuel;
	char* engine_load;
	char* air_temperature;

	char* stam;

} Commands;

int scanSerial(Connections* connections);
void freeConnections(Connections* connection);
int connectToELM327(char* portname, int *pd);
void printError(int error);
int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);
int sendELM(int pd, char* msg);
int recvELM(int pd, char* msg, int len);
int disconnect(int pd);
int OBD();
Commands getCommands();
int command(int pd, char* command,float* value ,char* unit);
int translateELMresponse(char* response , float* value , char* units);
int hexToDec(char* hex, int size);
int sendToSqlServer(char* values , int fd,int size);
int connectToSqlServer();