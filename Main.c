#include "OBD.h"


int main(void)
{
	int error, pd;
	char buf[MAX_MESSEGE_SIZE * sizeof(char)];
	creatLog();
	
	pd = OBD();
	if(pd < 0){return 0;}
	// --------------------------------------------------------------
	// OBD communication
	// --------------------------------------------------------------
	
	error = send(pd, "ATE0");
	if(error >= ERROR)	{disconnect(pd);return 0;}

	error = recv(pd, buf, MAX_MESSEGE_SIZE);
	//SprintError(error);
	if(error >= ERROR)	{disconnect(pd); return 0;}

	error = send(pd, "ATH1");
	//printError(error);
	if(error >= ERROR)	{disconnect(pd); return 0;}
	
	error = recv(pd, buf, MAX_MESSEGE_SIZE);
	//SprintError(error);
	if(error >= ERROR)	{disconnect(pd);  return 0;}


	error = send(pd, "ATE0");
	//printError(error);
	if(error >= ERROR)	{disconnect(pd);  return 0;}
	error = recv(pd, buf, MAX_MESSEGE_SIZE);
	//SprintError(error);
	if(error >= ERROR)	{disconnect(pd);  return 0;}

	error = send(pd, "ATH0");
	//printError(error);
	if(error >= ERROR)	{disconnect(pd); return 0;}
	error = recv(pd, buf, MAX_MESSEGE_SIZE);
	//SprintError(error);
	if(error >= ERROR)	{disconnect(pd); return 0;}

		error = send(pd, "ATRV");
	//printError(error);
	if(error >= ERROR)	{disconnect(pd); return 0;}
	error = recv(pd, buf, MAX_MESSEGE_SIZE);
	//SprintError(error);
	if(error >= ERROR)	{disconnect(pd); return 0;}

	disconnect(pd);
	closeLog();
	puts("done");
	return 0;
}
