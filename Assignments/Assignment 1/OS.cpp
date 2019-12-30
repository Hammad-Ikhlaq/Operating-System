#include<iostream>
#include<fstream>
#include<unistd.h>
#include<iostream>
#include<sys/wait.h>
#include<limits.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include<stdlib.h>
#include"minishell.h"
using namespace std;

int main(int argc, char*argv[])
{
	char CommandLine[50];
	char*parameter[20];
	command_t command;
	command.name=NULL;
	char*pathv[64];
	parsePath(pathv);
	char result[30];
	while(true)
	{
		
		printPrompt();
		readCommand(CommandLine);
		parseCommand(CommandLine, &command);
		command.name=lookupPath(command.argv,pathv,result);
		if(command.name==NULL)
		{
			cout<<"Command could not run\n";
		}
		else
		{
			pid_t pid;
			pid =fork();
			if(pid==0)
			{
				execv(command.name,command.argv);
				cout<<"Command could not run\n";	
			}
			wait(NULL);
		}
	}
return 0;
}

