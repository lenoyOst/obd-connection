#include "OBD.h"
#include "client.h"
#include "help.h"
#include <pthread.h>

void * runOBD(void* argv);

typedef struct args
{
	int *stop; // the thread will run until stop changes to 1 
	int Obdpd;
	int Sqlpd;
} Args;

int main(int argc, char** argv)
{
	int server_pd,obd_pd;
	char buf[MAX_MESSEGE_SIZE*sizeof(char)] = "";
	int port = 0;
	char* msg;
	int stop = 0;
	pthread_t tid;
	
	if(argc!= 6 || !isIp(argv[1]) || !onlyNumbers(argv[2]))
	{
		printf("wrong amount of args");
		return 0;
	}
	stringToInt(argv[2], &port);

	creatLog();
	obd_pd = OBD();
	if(obd_pd < 0){return 0;}
	
	server_pd = openClient(argv[1], port);
	
	Commands commands = getCommands();

	command(obd_pd, commands.echo_off, NULL ,buf);

	msg = appendStrings(6, "connect ", argv[3], " ", argv[4] , " ",argv[5]);
	write(server_pd, msg ,strlen(msg));
	if(read(server_pd ,msg , 3) == -1)
	{
		printf("error in read");
		return 0;
	}
	if(strncmp(msg ,"bye" ,3) == 0)
	{
		return 0;
	}

	Args arg = {&stop, obd_pd , server_pd};
    pthread_create(&tid, NULL, runOBD, (void*)&arg); 
	scanf("%d" ,&stop);
	stop = 1;
	disconnect(obd_pd);
	write(server_pd, "quit" ,4);
	closeLog();
	printf("BYE BYE!\n");
	return 0;
}

void * runOBD(void* args)
{
    Args* arg = (Args*)args;
	Commands commands = getCommands();
	char unit[20];
	float value =-1;
	int numOfVars = 6;
	char* Svalue = malloc(12);
	int size = sizeof(Svalue + 1) * numOfVars;
	char* values = (char*)malloc(size);
	printf("engine speed\tspeed\t\tthrotle\t\tengine load\tfuel\t\tair temperature\n");
    while(!(*(arg->stop)))
	{
		
		int error;
		if((error = command(arg->Obdpd, commands.rpm,&value ,unit))!=SUCCESS){
			if(error == NO_DATA_ERROR)
			{
				strcat(values ,"NULL ");
			}
			else break;
		}
		else
		{
			sprintf(Svalue ,"%f" ,value);
			strcat(values ,Svalue);
			strcat(values ," ");
		}

		if((error = command(arg->Obdpd, commands.speed,&value ,unit))!=SUCCESS)\
		{
			if(error == NO_DATA_ERROR)
			{
				strcat(values ,"NULL ");
			}
			else break;
		}
		else{
			sprintf(Svalue ,"%f" ,value);
			strcat(values ,Svalue);
			strcat(values ," ");
		}

		if((error = command(arg->Obdpd, commands.throtle,&value ,unit))!=SUCCESS){
			if(error == NO_DATA_ERROR)
			{
				strcat(values ,"NULL ");
			}
			else break;
		}
		else
		{
			sprintf(Svalue ,"%f" ,value);
			strcat(values ,Svalue);
			strcat(values ," ");
		}

		if((error = command(arg->Obdpd, commands.engine_load,&value ,unit))!=SUCCESS){	
			if(error == NO_DATA_ERROR)
			{
				strcat(values ,"NULL ");
			}
			else break;
		}
		else
		{
			sprintf(Svalue ,"%f" ,value);
			strcat(values ,Svalue);
			strcat(values ," ");
		}

		if((error = command(arg->Obdpd, commands.fuel,&value ,unit))!=SUCCESS)
		{
			if(error == NO_DATA_ERROR)
			{
				strcat(values ,"NULL ");
			}
			else 
			{
				printf("%d" ,error);
				break;
			}
		}
		else
		{
			sprintf(Svalue ,"%f" ,value);
			strcat(values ,Svalue);
			strcat(values ," ");
			printf("\t");
		}

		if((error = command(arg->Obdpd, commands.air_temperature,&value ,unit))!=SUCCESS){	
			if(error == NO_DATA_ERROR)
			{
				strcat(values ,"NULL");
			}
			else break;
		}
		else
		{
			sprintf(Svalue ,"%f" ,value);
			strcat(values ,Svalue);
		}
		puts(values);
		if(sendToSqlServer(values, strlen(values) ,arg->Sqlpd) >= ERROR) {break;}
		values[0] = '\0';
		printf("\n");
	}
	printf("\n");
	return NULL;
}