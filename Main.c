#include "OBD.h"
#include "client.h"
#include "help.h"

void * func(void* argv);

int main(int argc, char** argv)
{	
	int server_pd,obd_pd;
	char buf[MAX_MESSEGE_SIZE*sizeof(char)] = "";
	char unit[20];
	float value = -1;
	char Svalue[100];
	char values[100] = "";
	int port = 0;
	char* msg;
	
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
	printf("msg %s\n" , msg);
	if(strncmp(msg ,"bye" ,3) == 0)
	{
		return 0;
	}

	printf("engine speed\tspeed\t\tthrotle\t\tengine load\tfuel\t\tair temperature\n");
    while(1)
	{
		int error;
		if((error = command(obd_pd, commands.rpm,&value ,unit))!=SUCCESS){
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

		if(command(obd_pd, commands.speed,&value ,unit)!=SUCCESS){	if(error == NO_DATA_ERROR)
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

		if(command(obd_pd, commands.throtle,&value ,unit)!=SUCCESS){
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

		if(command(obd_pd, commands.engine_load,&value ,unit)!=SUCCESS){	
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

		if(command(obd_pd, commands.fuel,&value ,unit)!=SUCCESS)
		{
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
			printf("\t");
		}

		if(command(obd_pd, commands.air_temperature,&value ,unit)!=SUCCESS){	
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
		puts(values);
		if(sendToSqlServer(values, strlen(values) ,server_pd) >= ERROR) {break;}
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