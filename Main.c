#include "OBD.h"

int main(void)
{
	char** ports = scanSerial();
	printf("%s", ports[0]);
	free(ports);
}