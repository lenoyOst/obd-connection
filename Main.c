#include "OBD.h"


int main(void)
{
	int error, pd, i;
	Connections connections;
	char buf[MAX_MESSEGE_SIZE * sizeof(char)];
	creatLog();
	// --------------------------------------------------------------
	// scan serial ports - checks if there are connected serial ports
	// --------------------------------------------------------------
	
	error = scanSerial(&connections);
	//printError(error);
	if (error >= ERROR )	{return 0;}
	else if(connections.size==0)
	{
		puts("obd is not connected");
		return 0;
	}
	
	// ------------------------------------------------------------------------------------------------------------------------------------
	// connect to OBD - checks all the serial portsthat we found and finds the one that has ELM327 v1.5 connected , if we cound not fine one we exit
	// -------------------------------------------------------------------------------------------------------------------------------------
	
	for(i=0;i<connections.size;i++)
	{
		error = connect(connections.list[i], &pd);
		if(error == SUCCESS)
			break;
	}
	if(error!=SUCCESS)
	{
		puts("obd is not connected");
		freeConnections(&connections);
		return 0;
	}
	
	// --------------------------------------------------------------
	// OBD communication
	// --------------------------------------------------------------
	
	error = send(pd, "ATE0");
	//printError(error);
	if(error >= ERROR)	{disconnect(pd); freeConnections(&connections); return 0;}


	error = recv(pd, buf, MAX_MESSEGE_SIZE);
	//SprintError(error);
	if(error >= ERROR)	{disconnect(pd); freeConnections(&connections); return 0;}
	puts(buf);

	error = send(pd, "ATH1");
	//printError(error);
	if(error >= ERROR)	{disconnect(pd); freeConnections(&connections); return 0;}
	
	error = recv(pd, buf, MAX_MESSEGE_SIZE);
	//SprintError(error);
	if(error >= ERROR)	{disconnect(pd); freeConnections(&connections); return 0;}
	puts(buf);


	error = send(pd, "ATE0");
	//printError(error);
	if(error >= ERROR)	{disconnect(pd); freeConnections(&connections); return 0;}
	error = recv(pd, buf, MAX_MESSEGE_SIZE);
	//SprintError(error);
	if(error >= ERROR)	{disconnect(pd); freeConnections(&connections); return 0;}
	puts(buf);

	error = send(pd, "ATH0");
	//printError(error);
	if(error >= ERROR)	{disconnect(pd); freeConnections(&connections); return 0;}
	error = recv(pd, buf, MAX_MESSEGE_SIZE);
	//SprintError(error);
	if(error >= ERROR)	{disconnect(pd); freeConnections(&connections); return 0;}
	puts(buf);

		error = send(pd, "ATRV");
	//printError(error);
	if(error >= ERROR)	{disconnect(pd); freeConnections(&connections); return 0;}
	error = recv(pd, buf, MAX_MESSEGE_SIZE);
	//SprintError(error);
	if(error >= ERROR)	{disconnect(pd); freeConnections(&connections); return 0;}
	puts(buf);

	freeConnections(&connections);
	disconnect(pd);
	closeLog();
	puts("done");
	return 0;
}
