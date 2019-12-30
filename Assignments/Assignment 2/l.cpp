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
	
	while(true)
	{int i=1;
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
					
					if(redirection_counter==2)//if two redirections are present
					{	
							int a[2],b[2];
							pipe(a);
							pipe(b);
							i=index_red(command.argv,command.argc,i);
							pid_t pid=fork();
							if(pid==0)
							{
								pid_t pid=fork();
								if(pid==0)
								{
									command_t temp;
									temp=get_bcom(command,i);
									temp.name=lookupPath(temp.argv,pathv,result);
									if(*command.argv[i]=='|')
									{
									close(a[0]);
									dup2(a[1],1);//writing redirection only
									close(b[0]);
									close(b[1]);
									}
									if(*command.argv[i]=='>')
									{
									
									close(a[1]);
									close(b[0]);
									close(b[1]);
									int fd=open(command.argv[i+1], O_WRONLY);
																				
										dup2(a[0],0);//reading redirection
										
										dup2(fd,1);//writing in file
										
										
									}
									execv(temp.name,temp.argv);
									cout<<temp.name<<"Command could not run\n";
									return 0;
								}
								else if(pid>0)
								{
									pid =fork();
									if(pid==0)
									{
									i++;
									i=index_red(command.argv,command.argc,i);
									command_t temp;
									if(*command.argv[i]=='|')
									{
									
									temp=get_acom(command,i);
									temp.name=lookupPath(temp.argv,pathv,result);
									
									close(b[1]);
									dup2(b[0],0);//reading redirection only
									close(a[0]);
									close(a[1]);
									}
									if(*command.argv[i]=='>')
									{
									
									temp=get_bcom(command,i);
									temp.name=lookupPath(temp.argv,pathv,result);
									close(a[1]);
									close(a[0]);
									close(b[1]);
									int fd=open(command.argv[i+1], O_WRONLY);
																				
										dup2(b[0],0);//reading redirection
										
										dup2(fd,1);//writing in file
										
										
									}
									execv(temp.name,temp.argv);
									cout<<temp.name<<"Command could not run\n";
									return 0;
									}
									else if(pid>0)
									{
									
									command_t temp;
									temp=get_acom(command,i);
									temp.name=lookupPath(temp.argv,pathv,result);
									
									close(a[1]);
									dup2(a[0],0);//reading redirection
									close(b[0]);
									dup2(b[1],1);//writing redirection
									
									
									execv(temp.name,temp.argv);
									cout<<temp.name<<"Command could not run\n";
									return 0;
									}
								}
							}
							else
							{
								
									close(a[0]);
									close(a[1]);
									close(b[0]);
									close(b[1]);
									return 0;
							}	
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
				wait(NULL);
	}
return 0;
}

