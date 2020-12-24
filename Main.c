#include "OBD.h"

int main(void)
{
	int i;
	Connections* connections = scanSerial();
	if (connections == NULL)
		return 0;

	for (i = 0; i < connections->size; i++)
	{
		puts(connections->list[i]);
	}
	free(connections->list);
	return 0;
}