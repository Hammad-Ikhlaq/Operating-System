#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include <arpa/inet.h>
#include<netinet/in.h>
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
#include <sys/stat.h>
#include <fcntl.h>
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
	int redirection_counter=0;//to count indirection
	int i=1;
	while(true)
	{
		pid_t ppid=fork();
		if(ppid==0)
		{
			printPrompt();
			readCommand(CommandLine);
			parseCommand(CommandLine, &command);
			command.name=lookupPath(command.argv,pathv,result);
			if(command.argc>1)//if there is some parameter
			{
			redirection_counter=count_red(command.argv,command.argc);//counting the redirection
			
			}
				if(command.name==NULL)
				{
					cout<<"Command could not run\n";
				}
				else
				{	
					if(redirection_counter>0)//if redirection is present
					{	
								int a[2];
								pipe(a);
								int flag=0;
								redirection_counter++;
								cout<<"redirection counter "<<redirection_counter<<endl;
							while(redirection_counter>=1)//until only one redirection is left
							{
								i=index_red(command.argv,command.argc,i);//give me index of redirection at or after i
								cout<<"index = "<<*command.argv[i]<<endl;
								if(i==0)//if all redirections are gone
								exit(0);
								pid_t pid=fork();
								if((pid==0)&&(*command.argv[i]=='|'))
								{
												
										
										command_t temp;
										if(flag==0)//for first command
										temp=get_bcom(command,i);//get command before redirection
										else
										{
										temp=get_acom(command,i);//get command after redirection
										}
										
										temp.name=lookupPath(temp.argv,pathv,result);
										if(flag==0)//not for last command
										{
										cout<<"chanign writing redirection"<<endl;
										cout<<"temp.name = "<<temp.name<<endl;
										dup2(a[1],1);//writing redirection
										close(a[1]);
										close(a[0]);
										}
										if(flag==1)//not for first command
										{
										
										dup2(a[0],0);//reading redirection
										close(a[1]);
										close(a[0]);
										}
										
										execv(temp.name,temp.argv);
										cout<<temp.name<<"Command could not run\n";
										return 0;
									
								}
								else if(*command.argv[i]=='>'&&pid==0) //if > redirection is present
								{
										command_t temp;
										temp=get_bcom(command,i);//get command before redirection
										temp.name=lookupPath(temp.argv,pathv,result);
										int fd=open(command.argv[i+1], O_WRONLY);
										
										if(flag!=0)//for first command
										dup2(a[0],0);//reading redirection
										
										dup2(fd,1);//writing in file
										execv(temp.name,temp.argv);
										cout<<temp.name<<"Command could not run\n";
										return 0;
								}
								else if(pid>0)
								{
									
									if(flag==0)
									flag=1;//first command is over
									if(flag==1)
										{
											redirection_counter--;
											//i++;
										}
										if(redirection_counter<1)
										{
										close(a[1]);
										close(a[0]);
										}
										wait(NULL);
								}
							}//end of while
					}
					else//commands without redirection
					{
						pid_t pid;
						pid =fork();
						if(pid==0)
						{
							execv(command.name,command.argv);
							cout<<"Command could not run\n";
							return 0;	
						}
					}
				}//end of command
			}
		else//minishell is waiting
		wait(NULL);
	}
return 0;
}

