#define CRT_SECURE_NO_WARNINGS
#define LINE_LEN 80
#define MAX_ARGS 64
#define MAX_ARG_LEN 16
#define MAX_PATHS 64
#define MAX_PATH_LEN 96
#define WHITESPACE " .,\t\n"

#ifndef NULL
#define NULL
#endif

struct command_t
{
	char*name;
	int argc;
	char*argv[MAX_ARGS];
};

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
using namespace std;
void printPrompt();
int parseCommand(char*str, command_t*command);
void parsePath(char*dirs[]);
void readCommand(char* x);
char*lookupPath(char**argv, char**dir, char*result);
