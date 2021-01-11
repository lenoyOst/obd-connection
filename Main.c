#include "OBD.h"


int main(void)
{
	int pd;
	char buf[MAX_MESSEGE_SIZE*sizeof(char)] = "";
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

	disconnect(pd);
	closeLog();
	puts("done");

	return 0;
}
