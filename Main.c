#include "OBD.h"
#include <pthread.h>
void * func(void* argv);

typedef struct args
{
	int *stop; // the thread will run until stop changes to 1 
	int Obdpd;
	int Sqlpd;
} Args;

int main(void)
{
		
	int server_pd,obd_pd, stop = 0;
	char buf[MAX_MESSEGE_SIZE*sizeof(char)] = "";
	pthread_t tid; 
	
	creatLog();
	obd_pd = OBD();
	if(obd_pd < 0){return 0;}
	
	server_pd = connectToSqlServer();
	
	Commands commands = getCommands();

	command(obd_pd, commands.echo_off, NULL ,buf);
	Args arg = {&stop, obd_pd , server_pd};
    pthread_create(&tid, NULL, func, (void*)&arg); 
	pthread_join(tid , NULL);

	stop = 1;
	disconnect(obd_pd);
	write(server_pd, "quit" ,4);
	closeLog();
	printf("BYE BYE!\n");
	return 0;
}

void * func (void* args)
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
		
		if(command(arg->Obdpd, commands.rpm,&value ,unit)!=SUCCESS){break;}
		sprintf(Svalue ,"%f" ,value);
		strcat(values ,Svalue);
		strcat(values ," ");

		if(command(arg->Obdpd, commands.speed,&value ,unit)!=SUCCESS){break;}
		sprintf(Svalue ,"%f" ,value);
		strcat(values ,Svalue);
		strcat(values ," ");

		if(command(arg->Obdpd, commands.throtle,&value ,unit)!=SUCCESS){break;}
		sprintf(Svalue ,"%f" ,value);
		strcat(values ,Svalue);
		strcat(values ," ");

		if(command(arg->Obdpd, commands.engine_load,&value ,unit)!=SUCCESS){break;}
		sprintf(Svalue ,"%f" ,value);
		strcat(values ,Svalue);
		strcat(values ," ");

		if(command(arg->Obdpd, commands.fuel,&value ,unit)!=SUCCESS){break;}
		sprintf(Svalue ,"%f" ,value);
		strcat(values ,Svalue);
		strcat(values ," ");
		printf("\t");

		if(command(arg->Obdpd, commands.air_temperature,&value ,unit)!=SUCCESS){break;}
		sprintf(Svalue ,"%f" ,value);
		strcat(values ,Svalue);
		strcat(values ," ");

		sendToSqlServer(values ,arg->Sqlpd , size);
		printf("\n");
	}
	free(Svalue);
	free(values);
	printf("\n");
	return NULL;
}