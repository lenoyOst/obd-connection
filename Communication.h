#include<stdio.h>
#include<winsock2.h>

#define PORT 1212
#define IP "127.0.0.1"

#define SUCCESS 0
#define INITIALIZATION_ERROR 1
#define SOCKET_CREATION_ERROR 2
#define CONNECT_TO_SERVER_ERROR 3
#define SEND_ERROR 4

int startClient(char* ip, int port)
void printError(int error);