#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>


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
//				printf("%lld, ", atoll(token));
				idle += atoll(token);
			
			}
			else if (token != NULL)
			{
			
//				printf("%lld, ",atoll(token));
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
	
//	printf("\nlastTotal: %lld\ntotal: %lld\n lastIdle: %lld\n lastNoIdle: %lld\n idle: %lld\n noIdle: %lld\n", lastTotal, total, lastIdle, lastNoIdle, idle,noIdle);


	totald = total - lastTotal;
	idled = idle-lastIdle;
//	printf("totlad: %lld, idled: %lld\n", totald, idled);

	float  CPU_Usage = (float)(totald-idled)/(float)totald * 100;
	printf("CPU Usage: %f %%\n", CPU_Usage);
	
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
	printf("\n");
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
	getHostName();	
	return 0;
}
