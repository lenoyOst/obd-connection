#include "OBD.h"
#include <pthread.h>

void * func(void* argv);

typedef struct args
{
	int flag;
	int pd;
} Args;

int main(void)
{
	int pd, flag = 1;
	char buf[MAX_MESSEGE_SIZE*sizeof(char)] = "";
	pthread_t tid; 

	creatLog();
	
	pd = OBD();
	if(pd < 0){return 0;}
	// --------------------------------------------------------------
	// OBD communication
	// --------------------------------------------------------------
	
	Commands commands = getCommands();

	if(command(pd, commands.echo_off, buf)==SUCCESS)
		puts(buf);
	if(command(pd, commands.header_off, buf)==SUCCESS)
		puts(buf);

	Args arg = {1, pd};
    pthread_create(&tid, NULL, func, (void*)&arg); 

	scanf("%d", &flag);
	flag = 0;

	disconnect(pd);
	closeLog();
	return 0;
}

void * func (void* args)
{
    Args* arg = (Args*)args;
	Commands commands = getCommands();
	char buf[MAX_MESSEGE_SIZE*sizeof(char)] = "";

    while(arg->flag)
	{
		/*if(command(arg->pd, commands.rpm, buf)==SUCCESS)
			puts(buf);
		if(command(arg->pd, commands.speed, buf)==SUCCESS)
			puts(buf);
		if(command(arg->pd, commands.throtle, buf)==SUCCESS)
			puts(buf);
		if(command(arg->pd, commands.engine_load, buf)==SUCCESS)
			puts(buf);
		if(command(arg->pd, commands.fuel, buf)==SUCCESS)
			puts(buf);*/
		command(arg->pd, commands.echo_off, buf);
	}
	return NULL;
}