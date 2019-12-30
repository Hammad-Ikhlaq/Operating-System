#include<iostream>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include"mini.h"
using namespace std;

char* lookpath(char**argv,char**dirs,int& b)
{
	char* result;
	char* pname=new char[max_path_len];
	int val=-1;
	if(*argv[0]=='/')
	{
		for(int i=0;i<b;i++)
		{
			strcpy(pname,dirs[i]);
			strcat(pname,argv[0]);
			val=access(pname,F_OK);
			if(val==0)
			{
				return pname;
			}
			delete[]pname;
			pname=new char[max_path_len];		
		}
		return NULL;
	}
	result=new char[strlen(argv[0])+2];
	result[0]='/';
	strcat(result,argv[0]);
	for(int i=0;i<b;i++)
	{
		strcpy(pname,dirs[i]);
		strcat(pname,result);
		val=access(pname,F_OK);
		if(val==0)
		{
			delete[]result;
			return pname;
		}
		delete[]pname;
		pname=new char[max_path_len];
	}
	return NULL;
}

int parsecommand(char* com,command_t* command,int& cy)
{
	char * tok=new char[max_arg_len];
	strcpy(tok,com);
	int c=0,pch=0;
	int j=0;
	int i=0;
	for(;i<strlen(com);i++)
	{
		if(com[i]=='|'||com[i]=='>'||com[i]==' ')
		{
			command->argv[c]=new char[10];
			for(j=0;pch<i;j++)
			{
				if(com[pch]!='"')
					command->argv[c][j]=com[pch++];
				else
				{
					pch++;
					j--;
				}
			}
			command->argv[c++][j]='\0';
			if(com[i]!=' ')
			{
				j=0;
				cy++;
				command->argv[c]=new char[2];
				command->argv[c][j++]=com[pch++];
				command->argv[c++][j]='\0';
			}
			else
				pch++;
				
		}
	}
	command->argv[c]=new char[10];
	for(j=0;pch<i;j++)
		if(com[pch]!='"')
			command->argv[c][j]=com[pch++];
		else
		{
			pch++;
			j--;
		}
	command->argv[c++][j]='\0';
	return c;
}

int parsepath(char*dirs[])
{
	char* pathenvvar;
	char* thepath;
	
	for(int i=0; i<max_args;i++)
		dirs[i]=NULL;
	pathenvvar=(char*)getenv("PATH");
	int i=0;
	char * tok=strtok(pathenvvar,":");
	while(tok!=NULL)
	{
		dirs[i]=new char[strlen(tok)+1];
		dirs[i]=tok;
		tok=strtok(NULL,":");
		i++;
	}
	return i;
}

void printprompt()
{
	cout<<"Hi there: ";
}

void readcommand(char* str)
{
	cin.getline(str,100);
}

int main()
{
	char** pathv=new char*[100],*commandline=new char[100];
	int j=0,a=0,b=0;
	command_t command;
	char * pname=new char [max_path_len];
	a=parsepath(pathv);
	command.argv=new char*[max_args];
	int whi=0;
	int out = dup(1),in=dup(0);
	while(j<3)
	{
		printprompt();
		readcommand(commandline);
		whi=0;
		command.argc=parsecommand(commandline,&command,whi);
		command.argv[command.argc]=NULL;
		command.name=lookpath(command.argv,pathv,a);
		j++;
		int f=0;
		for(int i=0;i<command.argc;i++)
		{
			if(command.argv[i][0]=='>'||command.argv[i][0]=='|')
			{
				f=i;
				break;
			}
		}
		int k=0;
		while(k<=whi)
		{
		k++;
		if(command.name==NULL)
		{
			cout<<"command not found\n";
			continue;
		}
		else
		{	
			cout<<"the command name is:"<<command.name<<endl;
			if(f!=0)
			{
			if(command.argv[f][0]=='>')
			{
				
				char**arr=new char*[4];
				for(int i=0;i<f;i++)
				{
					arr[i]=new char[strlen(command.argv[i])];
					strcpy(arr[i],command.argv[i]);
				}
				for(int j=0,m=f+1;m<=command.argc;j++,m++)
				{
					command.argv[j]=command.argv[m];
				}
				command.argc=command.argc-(f+1);
				arr[f]=NULL;
				 int stdoutCopy = dup(1);	
   				 int out = open(command.argv[0],O_WRONLY|O_CREAT,0666); // Should also be symbolic values for access rights
   				
   				 if(out < 0)
					cout<<"File does not exist\n";
   				 int status = dup2(out,1);
    				if(status < 0)
					NULL;
    				pid_t pid=fork();
				if(pid==0)
				{	
					char *p=new char[10];
					p=command.argv[0];	
					execv(command.name,arr);	
				}
				else if(pid>0)
				{
					for(int i=0;i<4;i++)
						delete arr[i];
					delete[]arr;
					command.name=lookpath(command.argv,pathv,a);
					wait(NULL);
					dup2(in,0);
				}
    				close(out);
    				fflush(stdout);  
     
   				 status = dup2(stdoutCopy,1);
				k++;
			}
			else if(command.argv[f][0]=='|')
			{
				char**arr=new char*[4];
				for(int i=0;i<f;i++)
				{
					arr[i]=new char[strlen(command.argv[i])];
					strcpy(arr[i],command.argv[i]);
				}
				for(int j=0,m=f+1;m<=command.argc;j++,m++)
				{
					command.argv[j]=command.argv[m];
				}
				command.argc=command.argc-(f+1);
				arr[f]=NULL;
				int p[2];
				pipe(p);
				pid_t pid=fork();
				if(pid==0)
				{
					close(p[0]);
					dup2(p[1], 1); 
					execv(command.name,arr);
				}
				else if(pid>0)
				{
					command.name=lookpath(command.argv,pathv,a);
					for(int i=0;i<4;i++)
						delete arr[i];
					delete[]arr;
					wait(NULL);
					dup2(out,1);
					close(p[1]);
					dup2(p[0], 0);
				}
			}
			f=0;
			for(int i=0;i<command.argc;i++)
			{
				if(command.argv[i][0]=='>'||command.argv[i][0]=='|')
				{
					f=i;
					break;
				}
			}	
			}
			else
			{
				cout<<command.name<<endl;
				pid_t pid=fork();
				if(pid==0)
				{		
					execv(command.name,command.argv);	
				}
				else if(pid>0)
				{
					wait(NULL);
					dup2(in,0);
				}
				break;
			}
			}
		}		
	}
	return 0;	
}
