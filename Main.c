#include "OBD.h"
#include "client.h"
#include "help.h"

void * func(void* argv);

int main(int argc, char* argv)
{	
	int server_pd,obd_pd, stop = 0;
	char buf[MAX_MESSEGE_SIZE*sizeof(char)] = "";
	char unit[20];
	float value = -1;
	char Svalue[100];
	char values[100] = "";
	int port = 0;
	char* msg;
	
	if(argc!= 5 || !isIp(argv[1]) || !onlyNumbers(argv[2]))
	{
		return 0;
	}
	stringToInt(argv[2], &port);

	creatLog();
	obd_pd = OBD();
	if(obd_pd < 0){return 0;}
	
	server_pd = openClient(argv[1], port);

	Commands commands = getCommands();

	command(obd_pd, commands.echo_off, NULL ,buf);
	
	msg = appendStrings(4, "connect ", argv[3], " ", argv[4]);
	write(server_pd, msg ,4);

	printf("engine speed\tspeed\t\tthrotle\t\tengine load\tfuel\t\tair temperature\n");
    while(1)
	{
		
		if(command(obd_pd, commands.rpm,&value ,unit)!=SUCCESS){break;}
		sprintf(Svalue ,"%f" ,value);
		strcat(values ,Svalue);
		strcat(values ," ");

		if(command(obd_pd, commands.speed,&value ,unit)!=SUCCESS){break;}
		sprintf(Svalue ,"%f" ,value);
		strcat(values ,Svalue);
		strcat(values ," ");

		if(command(obd_pd, commands.throtle,&value ,unit)!=SUCCESS){break;}
		sprintf(Svalue ,"%f" ,value);
		strcat(values ,Svalue);
		strcat(values ," ");

		if(command(obd_pd, commands.engine_load,&value ,unit)!=SUCCESS){break;}
		sprintf(Svalue ,"%f" ,value);
		strcat(values ,Svalue);
		strcat(values ," ");

		if(command(obd_pd, commands.fuel,&value ,unit)!=SUCCESS){break;}
		sprintf(Svalue ,"%f" ,value);
		strcat(values ,Svalue);
		strcat(values ," ");
		printf("\t");

		if(command(obd_pd, commands.air_temperature,&value ,unit)!=SUCCESS){break;}
		sprintf(Svalue ,"%f" ,value);
		strcat(values ,Svalue);
		strcat(values ," ");

		sendToSqlServer(values, strlen(values) ,server_pd);
		values[0] = '\0';
		printf("\n");
	}

	disconnect(obd_pd);
	write(server_pd, "quit" ,4);
	closeClient(server_pd);

	closeLog();
	printf("BYE BYE!\n");
	return 0;
}