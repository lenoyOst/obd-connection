#include "OBD.h"


int scanSerial(Connections* connections)
{	
	writeToLog("-scanning for serial ports");
	int error = SUCCESS;
		

#ifdef _WIN32 

	int i;
	int current = 0;
	char** ports;

	
	HANDLE hCom;
	OVERLAPPED o;
	BOOL fSuccess;
	DWORD dwEvtMask;
	char text[11] = "\\\\.\\COM";

	ports = (char*)malloc(MAX_POTENTIALS_PORTS * sizeof(char));
	if (ports == NULL)
	{
		free(connections);
		return ALLOCATION_ERROR;
	}
		
	//going through all the possible ports (com 0-256)
	for (i = 0; i < 256; i++)
	{
		if (i < 10) { text[7] = i + '0'; }

		else if (i < 100) {
			text[7] = (i / 10) + '0';
			text[8] = (i % 10) + '0';
		}
		else
		{
			text[7] = (i / 100) + '0';
			text[8] = (i / 10) % 10 + '0';
			text[9] = (i % 10) + '0';
		}

		hCom = CreateFile(TEXT(text),
			GENERIC_READ | GENERIC_WRITE,
			0,    // exclusive access 
			NULL, // default security attributes 
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED,
			NULL
		);

	

		if (hCom == INVALID_HANDLE_VALUE)
		{
			//
		}
		else
		{
			ports[current] = _strdup(text);
			if (ports[current] == NULL)
			{
				error = ALLOCATION_WARNING;
				break;

			}

			current++;
		}
		if (current == MAX_POTENTIALS_PORTS)
		{
			error = RICHED_MAX_POTENTIALS_PORTS_WARNING;
			break;
		}
	}
	
	connections->list = ports;
	connections->size = current;
#elif __linux__
	glob_t globlist;
	glob("/dev/ttyS[0-9]", 0, NULL, &globlist);
	glob("/dev/ttyUSB[0-9]*", GLOB_APPEND, NULL, &globlist);

	connections->list = globlist.gl_pathv;
	connections->size = globlist.gl_pathc;

#endif
	return error;
}

// -------------------------------------------------------------
// releases the potential serial-port list(possible connections)
// -------------------------------------------------------------

void freeConnections(Connections* connection)
{
	int i;
	
	for(i=0;i<connection->size;i++)
	{
		free(connection->list[i]);
	}
	free(connection->list);
}

//---------------------------------------------------
// trying to initialis communication with the elm327
//---------------------------------------------------

int connectToELM327(char* portname, int* pd)
{
	char sys[100] = "sudo chmod 777 ";
	char buf[MAX_MESSEGE_SIZE];
	char arr[MAX_MESSEGE_SIZE];
	int i;
	for(i=0;i<strlen(portname);i++)
	{
		sys[i+15] = portname[i];
	}
	system(sys);
	*pd = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);
	if (*pd < 0)
	{
		strcpy(arr, "-");
		strcat(arr, portname);
		strcat(arr, " is not obd");
		writeToLog(arr);
		return NOT_OBD;
	}
	set_interface_attribs (*pd, B38400, 0);
	set_blocking (*pd, 1);
	
	command(*pd, getCommands().reset,NULL, buf);
	if(buf == NULL)
	{
		disconnect(*pd);
		strcpy(arr, "-");
		strcat(arr, portname);
		strcat(arr, " is not obd");
		writeToLog(arr);
		return NOT_OBD;
	}
	if(strcmp(buf, "ATZ\n\n\nELM327 v1.5\n\n>") == 0 || strcmp(buf, "\n\nELM327 v1.5\n\n>") == 0) 
	{
		strcpy(arr, "-obd found: ");
		strcat(arr, portname);
		writeToLog(arr);
		return SUCCESS;
	}
	disconnect(*pd);
	strcpy(arr, "-unexpected answer from "); 
	strcat(arr, portname);
	writeToLog(arr);
	return NOT_OBD;
}
// --------------------------
// disconnect 
// --------------------------
int disconnect(int pd)
{
	if(close(pd)<0)
	{
		writeToLog("-closing port error");
		return CLOSE_ERROR;
	}
	writeToLog("-port closed");
	return SUCCESS;
}

// --------------------
// send's messege to obd
// --------------------

int sendELM(int pd, char* msg)
{
	int len = strlen(msg), i;
	char text[len+3];
	for(i=0;i<len;i++)
	{
		text[i] = msg[i];
	}
	text[i] = '\r';	
	if(write (pd, text, strlen(text)) < 0)
	{
		writeToLog("-write to port error");
		return WRITE_ERROR;
	}
	return SUCCESS;
}
// --------------------------------------------------------------------
// reads a massege from the connection that we created with the elm327
// --------------------------------------------------------------------
int recvELM(int pd, char* msg, int len)
{
	int n;
	n = read (pd, msg, len);
	if(n<0)	{writeToLog("-exited read with -1");return READ_ERROR;}
	if(strncmp(msg , "7F 01 22" ,8) == 0 || strncmp(msg , "NO DATA" ,7) == 0)
	{
		writeToLog("engine is off");
		return ENGINE_IS_OFF;
	}
	if(strcmp(msg , "CAN ERROR\n\n>") == 0)
	{
		return CAR_NOT_CONNECTED_ERROR;
	}
	else if(strncmp(msg , "SE" ,2) == 0)// in case of searching
	{
		sleep(2);
		n += read (pd, msg + n, len);
	}
	msg[n] = '\0';
	if(strcmp(msg , "SEARCHING...\nUNABLE TO CONNECT\n\n>") == 0){writeToLog(msg); return ENGINE_IS_OFF;}
	return SUCCESS;
}
int recvELM2(int pd, char* msg, int len)
{
	int n = 0, bytes = 0;

	while(n==0 || msg[n-1] != '>')
	{
		bytes = read (pd, msg + n, len - n);
		if(bytes>=0)n+=bytes;
	}

	msg[n] = '\0';

	if(strcmp(msg , "CAN ERROR\n\n>") == 0){return ENGINE_IS_OFF;}
	if(strcmp(msg , "SEARCHING...\nUNABLE TO CONNECT\n\n>") == 0){return ENGINE_IS_OFF;}
	if(strncmp(msg , "7F 01 22" ,8) == 0 || strncmp(msg , "NO DATA" ,7) == 0)
	{
		writeToLog("engine is off");
		return ENGINE_IS_OFF;
	}
	return SUCCESS;
}

int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        if (tcgetattr (fd, &tty) != 0)
        {
		 printf("error0");
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
		 printf("error1");
                return -1;
        }
        return 0;
}

void set_blocking (int fd, int should_block)

{
        struct termios tty;
        memset (&tty, 0, sizeof(tty));
        if (tcgetattr (fd, &tty) != 0)
        {
		printf("error2");
               return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
		printf("error3");
}

// -------------------------------------------------
//finds the serial port that the obd is connected to
// -------------------------------------------------
int OBD()
{
	Connections connections;
	int error , i ,pd;
	// --------------------------------------------------------------
	// scan serial ports - checks if there are connected serial ports
	// --------------------------------------------------------------
	error = scanSerial(&connections);
	if (error >= ERROR )	{return 0;}
	else if(connections.size==0)
	{
		writeToLog("-obd is not connected");
		freeConnections(&connections);
		return -1;
	}
	// ------------------------------------------------------------------------------------------------------------------------------------
	// connect to OBD - checks all the serial ports that we found and finds the one that has ELM327 v1.5 connected , if we cound not fine one we exit
	// -------------------------------------------------------------------------------------------------------------------------------------
	
	for(i=0;i<connections.size;i++)
	{
		error = connectToELM327("/dev/ttyS3", &pd);
		if(error == SUCCESS)
			break;
	}
	if(error!=SUCCESS)
	{
		writeToLog("-obd is not connected");
		freeConnections(&connections);
		return -1;
	}
	freeConnections(&connections);
	
	return pd;
}
//-------------------------------------
//return a struct that contains the commands 
//--------------------------------
Commands getCommands()
{
	Commands commands;

	commands.engine_inputLevel = "ATIGN";
	commands.echo_off="ATE0";
	commands.echo_on="ATE1";
	commands.header_off="ATH0";
	commands.header_on="ATH1";
	commands.reset="ATZ";
	commands.describe_protocol="ATDP";
	commands.timing_off="ATAT0";
	commands.timing_auto1="ATAT1";
	commands.timing_auto2="ATAT2";
	commands.linefeeds_off="ATL0";
	commands.linefeeds_on="ATL1";
	commands.set_protocol_auto="ATSP0";
	commands.monitor = "ATMA";
	commands.config_monitor = "ATPBC001";
	commands.long_msg_ON = "ATAL";
	commands.protocolJ1939 = "ATSPB";
	commands.stam = "stam";
	
	commands.rpm = "010C";
	commands.speed = "010D";
	commands.throtle = "0111";
	commands.fuel = "012F";
	commands.engine_load = "0104";
	commands.air_temperature = "010F";
	commands.Drivers_throtlePrecent = "0161";
	commands.Accelerator_pedal_position = "014A";
	commands.aux_connected = "011E";


	return commands;
}
//------------------------------------------------------------------------------------
// send to the elm the command , reads from the elm the answer and sets the answer var
//------------------------------------------------------------------------------------
int command(int pd, char* command,float* value ,char* unit)
{
	int error = SUCCESS;
	char buf[MAX_MESSEGE_SIZE * sizeof(char)];

	error = sendELM(pd, command);
	if(error >= ERROR)	{return error;}

	error = recvELM2(pd, buf, MAX_MESSEGE_SIZE);
	if(error >= ERROR)	{return error;}
	if(strncmp(command, "AT", 2) == 0)
	{
		strcpy(unit, buf);
		return SUCCESS;
	}

	error = translateELMresponse(buf ,value ,unit);
	if(error == -1)
	{
		writeToLog("ERROR from translate\n");
		return NO_DATA_ERROR;
	}
	else if(error == -2)
	{
		writeToLog("NO DATA from translate\t");
		return NO_DATA_ERROR;
	}
	else
	{
		//printf("%f%s\t", *value, unit);
	}

	return SUCCESS;
}
// --------------------------------------------------------------------------------------------------------
//translet a response that was origanily recived from the ELM327 to the value in dec and the right unit of data
// --------------------------------------------------------------------------------------------------------
int translateELMresponse(char* response , float* value , char* units)
{
	int val;
	if(strcmp(response, "NO DATA\n\n>")==0 )
	{
		return -2;
	}
	if(strncmp(response,"41", 2)!=0)	{return -1;} // 41 resambles a correct answer
	
	val = hexToDec(response+3,2);
	
	switch (val)
	{
		case 12: //engine speed(0C)
			*value = hexToDec(response+6, 5)/4.0;
			strcpy(units,"rpm");
			break;
		case 13: // speed (0D)
			*value = hexToDec(response+6, 2);
			strcpy(units,"Kmph");
			break;
		case 17:  
		case 4 :
		case 47: //throtle/ engine_load/ fual
			*value = (hexToDec(response+6, 2)*100)/255.0;
			units[0] = '%';
			units[1] = '\0';
		case 15 : //air tempartaure
			*value = hexToDec(response+6, 2)-40;
			units[0] = 'c';
			units[1] = '\0';
			break;
		case 97: 
		case 98:  
			*value = hexToDec(response+6, 2)-125;
			units[0] = '%';
			units[1] = '\0';
			break;
		case 30:
			break;
		case 74 :
		case 75 :
		case 76 :
			*value = (100.0/255.0) * hexToDec(response+6, 2);
			units[0] = '%';
			units[1] = '\0';
			break;

	default:
		break;
	}
	return SUCCESS;
}

int hexToDec(char* hex, int size)
{
	int i , pow=1;
	int answer = 0;
	for (i =size-1; i >= 0; i--)
	{
		if(hex[i] >= '0' && hex[i]<='9')
		{
			answer+=(hex[i]-'0') * pow;
			pow*=16;
		}
		else if(hex[i] >= 'A' && hex[i]<='F')
		{
			answer+=((hex[i]-'A') + 10) * pow;
			pow*=16;
		}
	}
	return answer;
	
}

// ----------------------------------------
// upload data to server
// ----------------------------------------
int sendToSqlServer(char* values,int size , int fd)
{
	int n;
	char msg[size + 4];
	msg[0] = '\0';
	strcat(msg , "set ");
	strcat(msg , values);
	write(fd , msg ,size + 4);
	n = read(fd , msg ,3);
	msg[n] = '\0';
	writeToLog(msg);
	if(strncmp(msg , "bye",3) == 0)
	{
		return ERROR;
	}
	return SUCCESS;
}

//---------------------------
//gps
//---------------------------
void* gps(void* args)
{
	ArgsGPS *argsGPS = (ArgsGPS*)args;
	Connections connections;
    scanSerial(&connections);
	int pd;
	char line[MAX_MESSEGE_SIZE];
	Strings data;
    for(int i = 0;i<connections.size;i++)
    {
        if(connectToGPS("/dev/ttyS5", &pd) == SUCCESS)
        {
			
			while (!*(argsGPS->stop))
			{
				readLine(pd, line, MAX_MESSEGE_SIZE);
				data = split(line, ',');
				if(data.error == 1) return NULL;
				if(strcmp(data.strings[0], "$GPRMC") == 0)
				{
					if(strcmp(data.strings[2], "A") == 0)
					{
						
						strcpy(argsGPS->lat ,data.strings[3]);
						strcpy(argsGPS->latD ,data.strings[4]);
						strcpy(argsGPS->lon ,data.strings[5]);
						strcpy(argsGPS->lonD ,data.strings[6]);
					}
					else {
						strcpy(argsGPS->lat ,"NULL");
						strcpy(argsGPS->latD ,"NULL");
						strcpy(argsGPS->lon ,"NULL");
						strcpy(argsGPS->lonD ,"NULL");
					}
				}
			}
			freeStrings(data);
			break;
		}
	}
	return NULL;
}
int connectToGPS(char* portname, int* pd)
{
    char sys[100] = "sudo chmod 777 ";
    int i;
    for(i=0;i<strlen(portname);i++)
    {
        sys[i+15] = portname[i];
    }
    system(sys);
    *pd = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);
	if (*pd < 0)
    {
        return NOT_GPS;
    }
	
    set_interface_attribs (*pd, B9600, 0);
    set_blocking (*pd, 0);
    return SUCCESS;
}
void readLine(int pd, char* msg, int max_len)
{
    char c = '\0';
    int n;
    int total = 0;
    while(c!='\n' && total <= max_len)
    {
        n=-1;
        while(n!=1)
        {
            n = read(pd, msg+total, 1);
        }
        total+=n;
        c = msg[total-1];
    }
    msg[total-1] = '\0';
}