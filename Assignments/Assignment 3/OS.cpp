#include <iostream>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
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
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sysinfo.h>
using namespace std;

struct two_matrices
{
	int m,n,x,y,start_row,start_col,count,no_of_procs;
	int**a;
	int**b;
	int**c;
};
void* mul(void* para)
{
	two_matrices*box=(two_matrices*)para;
	int k = box->start_col;
	int i = box->start_row;
	for (; k < box->y; k++)
	{
	 	for (; i < box->m; i++)
		{
			if(box->count<=0)
			{
				pthread_exit( (void*) NULL);
			}
			for (int j = 0; j < box->x; j++)//j controls changing of enteries
			{
				box->c[i][k]+= box->a[i][j] * box->b[j][k];
			}
			box->count--;
		}
		i=0;
	}

  
}
int main(int argc, char*argv[])
{
	two_matrices box;
	ifstream fin1("Matrix1.txt");
	ifstream fin2("Matrix2.txt");
	ofstream fout("Result.txt");
	fin1>>box.m;
	fin1>>box.n;
	fin2>>box.x;
	fin2>>box.y;
	
	if(box.n!=box.x)
	{
		cout<<"Matrices are not multiplicable\n";
		return 0;
	}
	
	box.a=new int*[box.m];
	for(int j=0;j<box.m;j++)//Matrix 1 has m rows and n columns
	{
		box.a[j]=new int[box.n];
	}
	
	box.b=new int*[box.x];
	for(int j=0;j<box.x;j++)//Matrix 2 has x rows and y columns
	{
		box.b[j]=new int[box.y];
	}	
	for(int i=0;i<box.m;i++)//a has matrix 1
	{
		for(int j=0;j<box.n;j++)
		{
			fin1>>box.a[i][j];
		}
	}
	for(int i=0;i<box.x;i++)//b has matrix 2
	{
		for(int j=0;j<box.y;j++)
		{
			fin2>>box.b[i][j];
		}
	}
	box.c=new int*[box.m];
	for(int j=0;j<box.m;j++)
	{
		box.c[j]=new int[box.y];
	}
	box.no_of_procs=get_nprocs();
	pthread_t id[box.no_of_procs];
	int total_elements=box.m*box.y;
	box.count=total_elements/box.no_of_procs;	
	box.start_row=0;//initialize for first thread
	box.start_col=0;//initialize for first thread
	int mycount=box.count;
	for(int i=0; i<box.no_of_procs; i++)
	{
	box.count=total_elements/box.no_of_procs;
		if(i==box.no_of_procs-1)//for only last thread
		{
			box.count=total_elements/box.no_of_procs;
			box.count=box.count+(total_elements%box.no_of_procs);
		}
		pthread_create(&id[i], NULL, &mul, &box);
		pthread_join(id[i],(void**)NULL);
		box.start_col=0;
		box.start_row=mycount;//calculating new rows and cols
		while(box.start_row>=box.m)
		{
			box.start_row=box.start_row-box.m;
			box.start_col++;
		}
		mycount=mycount+(total_elements/box.no_of_procs);
	}
	for(int i=0;i<box.m;i++)//a has matrix 1
	{
		for(int j=0;j<box.y;j++)
		{
			fout<<box.c[i][j]<<" ";
		}
		fout<<endl;
	}
	
}
