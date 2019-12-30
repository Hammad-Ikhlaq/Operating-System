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

void printPrompt()
{
	char hname[50];
	string promptname;
	promptname="vmWare";
	char cwd[30];
	getcwd(cwd, sizeof(cwd));
	cout<<promptname<<"%"<<cwd<<">";
}

int parseCommand(char*str, command_t*command)
{
	char*token;
	token=strtok(str," ");
	command->argc=0;
	int i=0;
	if(token!=NULL)
	{
		command->argv[i]=token;
		i++;
		while(token!=NULL)
		{
			token=strtok(NULL," ");
			command->argv[i]= token;
			i++;
			command->argc++;
		}
	}
i++;
command->argv[i]=NULL;

}
void parsePath(char*dirs[])
{
	char*pathEnvVar;
	char*thePath;
	for(int i=0; i<MAX_ARGS; i++)
		dirs[i]=NULL;
	pathEnvVar=(char*)getenv("PATH");
	thePath=(char*)malloc(strlen(pathEnvVar)+1);
	strcpy(thePath, pathEnvVar);
	char*token;
	token=strtok(thePath,":");
	int i=0;
	if(token!=NULL)
	{
		dirs[i]=token;
		i++;
		while(token!=NULL)
		{
			token=strtok(NULL,":");
			dirs[i]=token;
			i++;
		}
	}
i++;
dirs[i]=NULL;
}
void readCommand(char* x)
{
	cin.getline(x,50);
}
char*lookupPath(char**argv, char**dir, char*result)
{

	char pName[MAX_PATH_LEN];
	int status;
	if(*argv[0]=='/')
	{
		status=access(argv[0],F_OK);
		if(status!=-1)
		return argv[0];
		return NULL;
	}
	for(int i=0;dir[i]!=NULL;i++)
	{
		strcpy(result,dir[i]); //result has path
		strcat(result,"/");
		strcat(result,argv[0]);
		status=access(result,F_OK);		
		if(status==0)
		{
			return result;
		}
	}
	fprintf(stderr, "%s:command not found\n", argv[0]);
	return NULL;
}
int count_red(char**argv, int argc)
{
	int i=1,j=0;
	while(i<argc)
	{
		if((*argv[i]=='|')||(*argv[i]=='>'))
		{
			j++;
		}
		i++;
	}
	return j;
}
int index_red(char**argv, int argc, int i)
{
	int j=i;
	while(j<argc)
	{
		if((*argv[j]=='|')||(*argv[j]=='>'))
		{
			return j;
		}
		j++;
	}
	return 0;
}
command_t get_bcom(command_t command,int i)
{
	command_t temp;
	int j=i-1;//seeking previous redirection
	while(j!=0&&*command.argv[j]!='|'&&*command.argv[j]!='>')
	{
		j--;
	}
	if(*command.argv[j]=='|'||*command.argv[j]=='>')
	j++;
	//now j contain the location of prev command
	temp.name=new char[10];
	strcpy(temp.name,command.argv[0]);
	
	int k=0;
	while(command.argv[j]!=NULL)
	{
		if(*command.argv[j]!='|'&&*command.argv[j]!='>')
		{
			temp.argv[k]=new char[10];
			strcpy(temp.argv[k],command.argv[j]);
		}
		k++;
		j++;
	}
	temp.argv[j+1]=NULL;	
	return temp;
}
command_t get_acom(command_t command,int i)
{
	int j=i+1;
	command_t temp;
	temp.name=new char[10];
	strcpy(temp.name,command.argv[j]);
	int k=0;
	while(command.argv[j]!=NULL)
	{
		if(*command.argv[j]!='|'&&*command.argv[j]!='>')
		{
			temp.argv[k]=new char[10];
			strcpy(temp.argv[k],command.argv[j]);
		}
		k++;
		j++;
	}
	temp.argv[j+1]=NULL;	
	return temp;
}
