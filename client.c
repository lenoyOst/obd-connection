#include "client.h"

// --------------------------------------------
// closes and relases the soceket that was open
// --------------------------------------------
void clean_up(int cond, int sock)
{
	close(sock);
	exit(cond);
}

// -------------------------------------------------------
// creating a new client that tries to connect to our server
// -------------------------------------------------------
int openClient(char* ip, int port)
{
	int sock;
	struct sockaddr_in client_name;
	
	sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock < 0)
	{
		exit(1);
	}
	bzero(&client_name,sizeof(client_name));
	client_name.sin_family = AF_INET;
	client_name.sin_addr.s_addr = inet_addr(ip);
	client_name.sin_port = htons(port);
	
	if(connect(sock, (struct sockaddr *)&client_name, sizeof(client_name)) < 0)
	{
		clean_up(2, sock);
	}
	
	return sock;
}

// -------------------
// closing the client
// -------------------
void closeClient(int sock) {
	close(sock);
}