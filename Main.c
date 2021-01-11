#include "OBD.h"


int main(void)
{
	int pd;
	char* buf;
	creatLog();
	
	pd = OBD();
	if(pd < 0){return 0;}
	// --------------------------------------------------------------
	// OBD communication
	// --------------------------------------------------------------
	
	Commands commands = getCommands();
	buf = command(pd, commands.echo_off);
	puts(buf);
	command(pd, commands.header_off);


	disconnect(pd);
	closeLog();
	puts("done");
	return 0;
}
