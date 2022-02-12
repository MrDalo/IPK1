#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>

void getHostName()
{
	FILE *hostName = fopen("/proc/sys/kernel/hostname", "rb");
	if(hostName == NULL)
	{
		fprintf(stderr, "Error in reading hostname\n");
		exit(1);
	}


	char chr;
	while(chr != '\n')
	{
		chr = fgetc(hostName);
		printf("%c", chr);

	}


	fclose(hostName);
}



void processorUsage()
{





}

void processorName()
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
					printf("%c", ch);
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
		fprintf(stderr, "Argument contains invalid character - it has to be integer only");
		return 1;
	}
	printf("port: %d\n", port);
	
	processorName();
	processorUsage();
	printf("\n");

	getHostName();	
	return 0;
}
