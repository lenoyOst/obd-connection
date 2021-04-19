#include "OBD.h"
#include "client.h"
#include <pthread.h>

void * runOBD(void* argv);

void * runOBD1(void* args);

typedef struct args
{
	int *stop; // the thread will run until stop changes to 1 
	int Obdpd;
	int Sqlpd;
	ArgsGPS argsGPS;
} Args;

int main(int argc, char** argv) // argv : ip port carID customerID password
{
	char *lat = (char*)malloc(20);
	char *lon = (char*)malloc(20);
	char *latD = (char*)malloc(2);
	char *lonD = (char*)malloc(2);
	int server_pd,obd_pd;
	char buf[MAX_MESSEGE_SIZE*sizeof(char)] = "";
	int port = 0;
	char* msg;
	int stop = 0;
	pthread_t tid[2];

	strcpy(lat,"NULL");
	strcpy(latD,"NULL");
	strcpy(lon,"NULL");
	strcpy(lonD,"NULL");

	if(argc!= 6 || !isIp(argv[1]) || !onlyNumbers(argv[2]))
	{
		puts("wrong amount of args");
		return 0;
	}
	if(stringToInt(argv[2], &port) < 0)
	{
		puts("port must be a int");
		return 0;
	}
	creatLog();
	if((obd_pd = OBD()) < 0){return 0;}
	
	if((server_pd = openClient(argv[1], port)) < 0)
	{
		writeToLog("could not connect to server");
		return 0;
	}
	
	//identify to the dataBase server and connect to the correct user 
	msg = appendStrings(6, "connect ", argv[3], " ", argv[4] , " ",argv[5]); // syntax : connect carID UserID password 
	
	write(server_pd, msg ,strlen(msg));
	
	if(read(server_pd ,msg , 3) == -1)
	{
		writeToLog("error when trying to read server's respons after sending connect request");
		return 0;
	}
	if(strncmp(msg ,"bye" ,3) == 0)
	{
		writeToLog("server send 'bye' to disconnect");
		return 0;
	}

	Commands commands = getCommands();
	command(obd_pd, commands.echo_off, NULL ,buf); //echo_off

	ArgsGPS argGPS = {&stop,lat,latD , lon , lonD};
	Args arg = {&stop, obd_pd , server_pd ,argGPS};
	
    pthread_create(&tid[0], NULL, runOBD, (void*)&arg);
	pthread_create(&tid[1] , NULL ,gps , (void*)&argGPS);
	pthread_join(tid[0] , NULL);
	pthread_join(tid[1], NULL);
	//scanf("%d" ,arg.stop);
	*arg.stop = 1;
	disconnect(obd_pd); 
	write(server_pd, "quit" ,4); //disconnect from the server
	closeLog();
	return 0;
}
void * runOBD1(void* args)
{
	 Args* arg = (Args*)args;
	char values[MAX_MESSEGE_SIZE];
	while(!(*(arg->stop)))
	{
 strcat(values ,"NULL NULL NULL ");
		
		strcat(values ,arg->argsGPS.lat);
		strcat(values ," ");
		strcat(values ,arg->argsGPS.latD);
		strcat(values ," ");
		strcat(values ,arg->argsGPS.lon);
		strcat(values ," ");
		strcat(values ,arg->argsGPS.lonD);
	sendToSqlServer(values, strlen(values) ,arg->Sqlpd);
	values[0] = '\0';
	}
	
	
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
    while(!(*(arg->stop)))
	{
		int error;
		if((error = command(arg->Obdpd, commands.rpm,&value ,unit))!=SUCCESS){
			break;
		}
		else
		{
			sprintf(Svalue ,"%f" ,value);
			strcat(values ,Svalue);
			strcat(values ," ");
		}

		if((error = command(arg->Obdpd, commands.speed,&value ,unit))!=SUCCESS)
		{
		 break;
		}
		else{
			sprintf(Svalue ,"%f" ,value);
			strcat(values ,Svalue);
			strcat(values ," ");
		}

		if((error = command(arg->Obdpd, commands.Accelerator_pedal_position,&value ,unit))!=SUCCESS){
			 break;
		}
		else
		{
			sprintf(Svalue ,"%f" ,value);
			strcat(values ,Svalue);
			strcat(values ," ");
		}
		strcat(values ,arg->argsGPS.lat);
		strcat(values ," ");
		strcat(values ,arg->argsGPS.latD);
		strcat(values ," ");
		strcat(values ,arg->argsGPS.lon);
		strcat(values ," ");
		strcat(values ,arg->argsGPS.lonD);
		if(sendToSqlServer(values, strlen(values) ,arg->Sqlpd) >= ERROR) {break;}
		values[0] = '\0';
	}
	*(arg->stop) = 1;
	return NULL;
}