#include "Communication.h"

int startClient(char* ip, int port)
{
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	char* message;
	int flag = 0;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)	{return INITIALIZATION_ERROR;}

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)	{return SOCKET_CREATION_ERROR;}

	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0)	{return CONNECT_TO_SERVER_ERROR;}

	while (flag)
	{
		message = "information";
		if (send(s, message, strlen(message), 0) < 0)	{return SEND_ERROR;	}
	}

	return SUCCESS;

}

void printError(int error)
{
	switch (error)
	{
	case INITIALIZATION_ERROR:
		puts("failed initializing Winsock");
		break;
	case SOCKET_CREATION_ERROR:
		puts("failed creating socket");
		break;
	case CONNECT_TO_SERVER_ERROR:
		puts("failed connecting to server");
		break;
	case SEND_ERROR:
		puts("failed sending information");
		break;
	case SUCCESS:
		puts("success");
		break;
	default:
		puts("error undefined");
		break;
	}
}