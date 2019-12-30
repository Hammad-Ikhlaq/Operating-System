#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

#define NUM_BLOCKS 40
#define BLOCK_SIZE 50

#define RELIABILITY 0.95
#define PERIOD 2147483647.0
#define ERROR 1
#define NO_ERROR 0

using namespace std;

static int threshold;
static char*bList[NUM_BLOCKS];
static char CurrentDirectory[15]="OS";
static int fd=0;


void initDisk()//reading from txt file to make our own file system in block lists
{
	int i;
	char*token;
	ifstream fin("disk.txt");
	for(i=0; i<NUM_BLOCKS; i++)
	{
		bList[i]=new char[BLOCK_SIZE];
		char temp[BLOCK_SIZE];
		fin.getline(temp,BLOCK_SIZE);
		strcpy(bList[i],temp);
		token=strtok(temp,",");
		token=strtok(NULL,",");	//geting the second part if it is $ then the block is free
		
		if(strcmp(token,"$")==0)
		bList[i]=NULL;
		
	}
	threshold=(int)(RELIABILITY*PERIOD);
}
int dRead(int addr, char*buf)
{
	int i;
	char*bufPtr;
	
	if(addr>=NUM_BLOCKS)
	return ERROR;
	if(rand()>threshold)
	return ERROR;
	
	//we are assuming that addr is a block number
	
	if(bList[addr]!=NULL)
	{
		bufPtr=bList[addr];
		for(int i=0; i<BLOCK_SIZE; i++)
		buf[i]=*bufPtr++;
		
	}
	else
	{
		for(int i=0; i<BLOCK_SIZE; i++)
		buf[i]=0;
	}
	return NO_ERROR;
}
int dWrite(int addr, char*buf)
{
	int i;
	char*bufPtr;
	if(addr>=NUM_BLOCKS)
	return ERROR;
	if(rand()>threshold)
	return ERROR;
	if(bList[addr]==NULL)
	bList[addr]=new char[BLOCK_SIZE];
	bufPtr=bList[addr];
	for(int i=0; i<BLOCK_SIZE; i++)
	*bufPtr++=buf[i];
	return NO_ERROR;
}
int fLs()
{
	char*token,*token1;
	int my;
	int flag=0;
	char temp[BLOCK_SIZE];
	for(int i=0; i<NUM_BLOCKS; i++)
	{
		if(bList[i]!=NULL)
		{	
			
			strcpy(temp,bList[i]);	
			token=strtok(temp,",");
			token=strtok(NULL,",");	//now token may contain the file name

			static char*temp1=CurrentDirectory;
			if(strcmp(token,temp1)==0)//current directory file descriptor
			{
				
					token=strtok(NULL,",");
					token1=strtok(NULL,",");
					my=atoi(token1);
					strcpy(temp,bList[my-1]);	
					token=strtok(temp,",");
				while(flag==0)
				{
					token=strtok(NULL,",");
					cout<<token<<endl;
					token=strtok(NULL,",");//now token is checking if info is end
					token=strtok(NULL,",");
					if(atoi(token)==-1)
					flag=1;
					else
					{
						strcpy(temp,bList[atoi(token)-1]);	
						token=strtok(temp,",");
					}
				}
				return 0;
			}
		}
	}

}
int fOpen(char*name)//get file name and returns block number and set fd to that block number
{
	char*token=new char[BLOCK_SIZE];
	char*token1=new char[BLOCK_SIZE];
	for(int i=0; i<NUM_BLOCKS; i++)
	{
		if(bList[i]!=NULL)
		{	
			char temp[BLOCK_SIZE];
			strcpy(temp,bList[i]);	
			token1=strtok(temp,",");//token1 has block number
			token=strtok(NULL,",");	//now token may contain the file name

			static char*temp1=name;
			if(strcmp(token,temp1)==0)
			{
				token=strtok(NULL,",");//now token may contain the file type
				if(atoi(token)==2)//if it is a text file
				{
					token1=strtok(NULL,",");
					
					fd=atoi(token1);
					return atoi(token1);
				}
				
			}
		}
	}		
}

int fClose(int fileID)//restoring fd default
{
		fd=0;
		return NO_ERROR;
}
int fRead(int fileID, char*buffer, int length)//now fileID is block number of the content of file because of our fOpen
{
	if(bList[fileID-1]!=NULL)
	{	
		int i=0;
		char*token=new char[BLOCK_SIZE];
		char temp[BLOCK_SIZE];
		strcpy(temp,bList[fileID-1]);	
		token=strtok(temp,",");//token1 has block number
		token=strtok(NULL,",");	//now token may contain the file content
		int x;
		if(length<BLOCK_SIZE)
		x=length;
		else
		x=BLOCK_SIZE;
		for(;i<x;i++)
		{
		buffer[i]=token[i];
		}
		buffer[i]='\0';
		return x;
	}
	return ERROR;
}
int fWrite(int fileID, char*buffer, int length)
{
	if(bList[fileID-1]!=NULL)
	{	
		int flag=0;
		int i=0;
		char*token=new char[BLOCK_SIZE];
		char*token1=new char[BLOCK_SIZE];
		char temp[BLOCK_SIZE];
			
		while(flag==0)
		{
			strcpy(temp,bList[fileID-1]);
			token1=strtok(temp,",");
			token=strtok(NULL,",");
			token=strtok(NULL,",");
			token=strtok(NULL,",");//token checks if the content has ended
			if(atoi(token)==-1)
			{
			flag=1;
			}
			else
			fileID=atoi(token);
		}
		//now token1 has block number of last block
		int done=0;
		while(done==0)
		{
			char*free=new char[1];
			*free='&';
			
			for(int i=0; i<NUM_BLOCKS; i++)
			{
				if(bList[i]==NULL)
				{
					sprintf(free, "%d", i+1);
					break;
				}
			}
			
			if(strcmp(free,"&")==0)
			return ERROR;
			else
			{
				int coma=3;
				for(int i=0; i<BLOCK_SIZE&&coma!=0; i++)
				{
					if(bList[atoi(token1)-1][i]==',')
					{
						coma--;
						if(coma==0)
						{
							
							bList[atoi(token1)-1][i+1]=(*free);//removing block number from end
							cout<<free<<endl;
							break;
						}
					}
				}
				int j=1;
				bList[atoi(free)-1]=new char[BLOCK_SIZE];
				bList[atoi(free)-1][0]=free[0];
				bList[atoi(free)-1][1]=free[1];
				bList[atoi(free)-1][2]=',';
				for(; j<BLOCK_SIZE-6; j++)
				{
					bList[atoi(free)-1][2+j]=buffer[j-1];	
				}
				bList[atoi(free)-1][2+j]=',';
				bList[atoi(free)-1][3+j]='4';
				bList[atoi(free)-1][4+j]=',';
				bList[atoi(free)-1][5+j]='-';
				bList[atoi(free)-1][6+j]='1';
				bList[atoi(free)-1][7+j]='\0';
				cout<<bList[atoi(free)-1]<<endl;
				done=-1;
				
			}
		}
	}
	return ERROR;
}
int fSeek(int fileID, int position)
{
	if(position<BLOCK_SIZE)
	return bList[fileID-1][position];
	
	return ERROR;
}
int fmkdir(char*name)
{
	
	char*token,*token1,*last;
	int my;
	int flag=0;
	char temp[BLOCK_SIZE];
	for(int i=0; i<NUM_BLOCKS; i++)
	{
		if(bList[i]!=NULL)
		{	
			
			strcpy(temp,bList[i]);	
			token=strtok(temp,",");
			token=strtok(NULL,",");	//now token may contain the file name

			static char*temp1=CurrentDirectory;
			if(strcmp(token,temp1)==0)//current directory file descriptor
			{
				
					token=strtok(NULL,",");
					token1=strtok(NULL,",");
					my=atoi(token1);
					strcpy(temp,bList[my-1]);	
					token=strtok(temp,",");
				while(flag==0)
				{
					last=strtok(NULL,",");
					cout<<token<<endl;
					token=strtok(NULL,",");//now token is checking if info is end
					token=strtok(NULL,",");
					if(atoi(token)==-1)
					flag=1;
					else
					{
						strcpy(temp,bList[atoi(token)-1]);	
						token=strtok(temp,",");
					}
				}
				int coma=2;
				for(int i=0; i<BLOCK_SIZE&&coma!=0; i++)
				{
					if(bList[atoi(last)-1][i]==',')
					{
						coma--;
						if(coma==0)
						{
							for(int j=0; j<strlen(name);j++,i++)
							bList[atoi(last)-1][i+1]=name[j];
							bList[atoi(last)-1][i+1]=',';i++;
							bList[atoi(last)-1][i+1]='3';i++;
							bList[atoi(last)-1][i+1]=',';i++;
							bList[atoi(last)-1][i+1]='-';i++;
							bList[atoi(last)-1][i+1]='1';i++;
							return 0;
						}
					}
				}
				
				
			}
		}
	}
}
int fCd(char*name)
{
	char*token=new char[BLOCK_SIZE];
	char*token1=new char[BLOCK_SIZE];
	for(int i=0; i<NUM_BLOCKS; i++)
	{
		if(bList[i]!=NULL)
		{
			char temp[BLOCK_SIZE];
			strcpy(temp,bList[i]);
			token=strtok(temp,",");
			token1=strtok(NULL,",");//geting the second part if it is $ then the block is free
			if(strcmp(name,token1)==0)//name matches
			{
				token=strtok(NULL,",");
				if(atoi(token)==1)//attribute matches
				{
					
					strcpy(CurrentDirectory,token1);
					return NO_ERROR;
				}
			}
		}
	}
	return ERROR;	
}
int main()
{
	initDisk();
	
	char*name=new char[BLOCK_SIZE];
	strcpy(name,"29,temp,1,29");
	//dWrite(28,name);
	//char*buf=new char[BLOCK_SIZE];
	//dRead(28,buf);
	//strcpy(name,"sample");
	//fCd(name);
	//fLs();
	char*buffer=new char[100];
	//cout<<fRead(23, buffer, 100)<<endl;
	//cout<<buffer<<endl;
	
	strcpy(name,"Fakhar");
	fOpen(name);
	strcpy(buffer,"Kubuntu is a user friendly operating system based on KDE, the K Desktop Environment.");
	fWrite(fd,buffer,strlen(buffer));
	
}		
