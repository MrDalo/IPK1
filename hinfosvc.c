#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>


void getHostName(char name[])
{
	FILE *hostName = fopen("/proc/sys/kernel/hostname", "rb");
	if(hostName == NULL)
	{
		fprintf(stderr, "Error in reading hostname\n");
		exit(1);
	}


	char chr;
	int i = 0;
	while(chr != '\n')
	{
		chr = fgetc(hostName);
		name[i] = chr;
		i++;
	}


	fclose(hostName);
}



void processorUsage(char usage[])
{
	int repeatForTimes = 2;
	
	char inputArray[4097];
	const char delime[] = " ";
	long long int i, noIdle, idle, lastNoIdle, lastIdle, total, lastTotal, totald, idled = 0;

	while(repeatForTimes > 0)
	{


		FILE *cpuStat = fopen("/proc/stat", "r");
		if(cpuStat == NULL)
		{
			fprintf(stderr, "Error in reading CPU stat file\n");
			exit(1);

		}
		
		fgets(inputArray, 4096, cpuStat);
		fclose(cpuStat);	
		char *token;
		token = strtok(inputArray, delime);
		i= 0;
		noIdle = 0;
		idle = 0;

		while(token != NULL)
		{
			token = strtok(NULL, delime);
			if( (i == 3 || i == 4) && token != NULL)
			{
				idle += atoll(token);
			}
			else if (token != NULL)
			{
				noIdle+=atoll(token);
			}
			i++;
		}
		

		if(repeatForTimes == 2)
		{
			lastIdle = idle;
			lastNoIdle = noIdle;
			sleep(1);
		}

		repeatForTimes --;
	}

	lastTotal = lastIdle + lastNoIdle;
	total = idle + noIdle;

	totald = total - lastTotal;
	idled = idle-lastIdle;

	float  CPU_Usage = (float)(totald-idled)/(float)totald * 100;
	sprintf(usage, "%.2f%%", CPU_Usage);

}

void processorName(char name[])
{
	FILE *cpuName = fopen("/proc/cpuinfo", "rb");
	if(cpuName == NULL)
	{
		fprintf(stderr, "Error in reading CPU name\n");
		exit(1);
	}

	char ch;
	int row = 0;
	bool doubleDot = false;
	int offset = 0;
	int i = 0;
	while(ch != EOF)
	{
		ch = fgetc(cpuName);
		if(ch == '\n')
			row++;

		if(row == 4)
		{
			if(ch == ':')
				doubleDot = true;

			if(doubleDot)
			{
				offset++;
				if(offset > 2)
				{
					name[i] = ch;
					i++;
				}
			}
		}
	}
	fclose(cpuName);
}



int portProcessing(int argc, char *argv[])
{
	if(argc != 2)
	{
		fprintf(stderr, "Argument ERROR\n");
		exit(1);
	}

	return atoi(argv[1]);
}


int main( int argc, char *argv[])
{
	int port = portProcessing(argc, argv);
	if(port == 0)
	{
		fprintf(stderr, "Argument contains invalid character - it has to be integer only\n");
		return 1;
	}

	int server_socket;
	int response_socket;
	if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
	{
		fprintf(stderr, "ERROR socket creating\n");
		exit(1);
	}

	int optval = 1;	
	if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, (const void *)&optval, sizeof(int)))
	{
		fprintf(stderr, "ERROR in setsockopt function\n");
		exit(1);
	}

	struct sockaddr_in server_address;
	int address_len = sizeof(server_address);

	server_address.sin_family = AF_INET;
	server_address.sin_port=htons((unsigned short)port);
	server_address.sin_addr.s_addr=INADDR_ANY;

	if(bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address))< 0)
	{
		fprintf(stderr, "ERROR in Binding\n");
		exit(1);
	}

	if(listen(server_socket, 3) == -1 )
	{
		fprintf(stderr, "ERROR listen failed\n");
		exit(1);
	}
	

	char buffer[4096] = {0};
	char *token;
	char delime[] = " ";
	char returnFunc[1024] = {0};
	char responseText[1024] = {0};
	while(true)
	{
	   memset(returnFunc, '\0', 1024);
		memset(responseText, '\0', 1024);
		if((response_socket = accept(server_socket, (struct sockaddr *)&server_address, (socklen_t*)&address_len))< 0 )
		{
			fprintf(stderr, "ERROR in accept user response\n");
			exit(1);
		}
		
		
		read(response_socket,buffer, 4096 );
		
		
		token =strtok(buffer, delime);
		token = strtok(NULL, delime);
		if(!strcmp(token, "/hostname"))
		{
			getHostName(returnFunc);			
			sprintf(responseText, "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n %s", returnFunc);
			send(response_socket, returnFunc, strlen(returnFunc), 0);

		}
		else if(!strcmp(token,"/cpu-name"))
		{
			processorName(returnFunc);
			sprintf(responseText, "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n %s", returnFunc);
			send(response_socket, returnFunc, strlen(returnFunc), 0);
		}
		else if(!strcmp(token, "/load"))
		{
			processorUsage(returnFunc);
			sprintf(responseText, "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n %s", returnFunc);
			send(response_socket, returnFunc, strlen(returnFunc), 0);
		}
		else
		{
			sprintf(returnFunc, "400 Bad Request\n");
			sprintf(returnFunc, "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain;\r\n\r\n Bad Request");
			send(response_socket, returnFunc, strlen(returnFunc), 0);

		}	



		close(response_socket);
	}


	return 0;
}
