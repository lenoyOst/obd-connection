#include "OBD.h"
#include <pthread.h>

void * func(void* argv);

typedef struct args
{
	int *stop; // the thread will run until stop changes to 1 
	int pd;
} Args;

int main(void)
{
	int pd, stop = 0;
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
	Args arg = {&stop, pd};
    pthread_create(&tid, NULL, func, (void*)&arg); 
	pthread_join(tid , NULL);

	stop = 1;
	disconnect(pd);
	closeLog();
	return 0;
}

void * func (void* args)
{
    Args* arg = (Args*)args;
	Commands commands = getCommands();
	char buf[MAX_MESSEGE_SIZE*sizeof(char)] = "";
    while(!(*(arg->stop)))
	{
		if(command(arg->pd, commands.rpm, buf)!=SUCCESS){break;}
		printf("rpm : %s\n" , buf);
		if(command(arg->pd, commands.speed, buf)!=SUCCESS){break;}
		printf("speed : %s\n" , buf);
		if(command(arg->pd, commands.throtle, buf)!=SUCCESS){break;}
		printf("throtle : %s\n" , buf);
		if(command(arg->pd, commands.engine_load, buf)!=SUCCESS){break;}
		printf("engine_load : %s\n" , buf);
		if(command(arg->pd, commands.fuel, buf)!=SUCCESS){break;}
		printf("fuel : %s\n" , buf);
	}
	return NULL;
}