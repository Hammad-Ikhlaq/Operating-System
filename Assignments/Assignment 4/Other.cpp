#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>

using namespace std;


static int NUM_BLOCKS = 40;
static int BLOCK_SIZE = 51;
#define ERROR 0
#define NO_ERROR 1
char CurrentDirectory[10] = "OS";

char **disk = new char *[NUM_BLOCKS];


int **blocks(char **arr);
int dRead(int addr, char*buf);
int dWrite(int addr, char*buf);
void print(char **disk);
int fOpen(char*name);
int fCd(char*name);
int fLs();

int main()
{
	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		disk[i] = new char[BLOCK_SIZE];
	}
	ifstream fin;
	int line = 0;
	char temp[51];
	fin.open("disk.txt");
	if (fin.is_open())
	{
		while (!fin.eof())
		{
			fin.getline(temp, 51, '\n');
			int length = strlen(temp);

			for(int i = 0; i<length; i++) //deep copy
				disk[line][i] = temp[i];
			disk[line][length] = '\0';
			line++;
		}
		//print(disk);
		
	}
	int **intArr = blocks(disk);

	/*for (int i = 0; i < NUM_BLOCKS; i++)
	{
		for (int j = 0; j < 2; j++)
			cout << intArr[i][j] << " ";
		cout << endl;
	}*/


	char*buf = new char[BLOCK_SIZE];
	if (dRead(20, buf) == ERROR)
		cout << "ERROR!" << endl;

	//dWrite(30, buf);

	int file = fOpen("Hammad");
	if (file == -1)
		cout << "No such file Exist." << endl;
	else cout << "File number is : " << file << endl;

	fCd("sample");
	//print(disk);

	fLs();

	for (int i = 0; i < NUM_BLOCKS; i++)
		delete[] disk[i];
	delete[] disk;
	system("pause");
	return 0;
}

int **blocks(char **arr)
{
	int **intArr = new int*[NUM_BLOCKS];
	char **temp = new char*[NUM_BLOCKS];
	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		temp[i] = new char[BLOCK_SIZE];
	}
	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		for (int j = 0; j < BLOCK_SIZE; j++)
		{
			temp[i][j] = disk[i][j];
		}
	}
	char *token = nullptr;
	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		intArr[i] = new int[2];
	}
	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		token = strtok(temp[i], ",");
		intArr[i][0] = atoi(token);
		token = strtok(NULL, ",");
		if (*token == '$')
		{
			intArr[i][1] = -1;
		}
		else
		{
			token = strtok(NULL, ",");
			token = strtok(NULL, ",");
			intArr[i][1] = atoi(token);
			token = strtok(NULL, ",");
		}
	}

	for (int i = 0; i < NUM_BLOCKS; i++)
		delete[] temp[i];
	delete[] temp;
	return intArr;
}

int dRead(int addr, char*buf)
{
	int i;
	char*bufPtr;

	if (addr >= NUM_BLOCKS)
		return ERROR;
	/*if (rand()>threshold)   //what is the use of this threshold!!
		return ERROR;*/

	//we are assuming that addr is a block number

	if (disk[addr] != NULL)
	{
		bufPtr = disk[addr-1];
		for (int i = 0; i < BLOCK_SIZE; i++)
			buf[i] = *bufPtr++;
	}
	else
	{
		for (int i = 0; i<BLOCK_SIZE; i++)
			buf[i] = 0;
	}
	return NO_ERROR;
}

int dWrite(int addr, char*buf)
{
	int i;
	char*bufPtr;
	if (addr >= NUM_BLOCKS)
		return ERROR;
	/*if (rand()>threshold)
		return ERROR;*/
	int start = 2;
	if (addr / 10 > 0)
		start = 3;
	bufPtr = disk[addr-1];  // minus 1 because in my virtual disk I start array from 1 not from 0
							//but actually arrays start from 0.
	for (int i = start; i<BLOCK_SIZE; i++)
		bufPtr[i] = buf[i];

	return NO_ERROR;
}

void print(char **disk)
{
	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		for (int j = 0; disk[i][j] != '\0'; j++)
			cout << disk[i][j];
		cout << endl;
	}
}

int fOpen(char*name)//get file name and returns block number
{
	char **temp = new char*[NUM_BLOCKS];
	char*token = nullptr;
	char*token1 = nullptr;
	if (disk != nullptr)
	{
		for (int i = 0; i < NUM_BLOCKS; i++)
		{
			temp[i] = new char[BLOCK_SIZE];
		}
		for (int i = 0; i < NUM_BLOCKS; i++)  //Deep copy because we are dealing with pointers.
		{
			for (int j = 0; j < BLOCK_SIZE; j++)
			{
				temp[i][j] = disk[i][j];
			}
		}
	}
	for (int i = 0; i<NUM_BLOCKS; i++)
	{
		if (temp[i] != NULL)
		{
			cout << temp[i] << endl;
			//Never use shallow copy when using strtok.

			token1 = strtok(temp[i], ",");//token1 has block number
			token = strtok(NULL, ",");	//now token may contain the file name

			char *temp1 = name;
			if (strcmp(token, temp1) == 0)
			{
				token = strtok(NULL, ",");//now token may contain the file type
				if (atoi(token) == 2)//if it is a text file
				{
					token1 = strtok(NULL, ",");
					int number = atoi(token1);
					for (int i = 0; i < NUM_BLOCKS; i++)
						delete[] temp[i];
					delete[] temp;
					return number;
				}

			}
		}
	}
	for (int i = 0; i < NUM_BLOCKS; i++)
		delete[] temp[i];
	delete[] temp;
	return -1;
}


int fLs()
{
	char **temp = new char*[NUM_BLOCKS];
	char*token = nullptr;
	char*token1 = nullptr;
	int address , flag = 0;
	if (disk != nullptr)
	{
		for (int i = 0; i < NUM_BLOCKS; i++)
		{
			temp[i] = new char[BLOCK_SIZE];
		}
		for (int i = 0; i < NUM_BLOCKS; i++)  //Deep copy because we are dealing with pointers.
		{
			for (int j = 0; j < BLOCK_SIZE; j++)
			{
				temp[i][j] = disk[i][j];
			}
		}
	}
	for (int i = 0; i<NUM_BLOCKS; i++)
	{
		if (disk[i] != NULL)
		{
			token = strtok(temp[i], ",");
			token = strtok(NULL, ",");	//now token may contain the file name

			static char*temp1 = CurrentDirectory;
			if (strcmp(token, temp1) == 0)//current directory file descriptor
			{

				token = strtok(NULL, ","); //now it contain type
				token1 = strtok(NULL, ",");
				address = atoi(token1);
				char*buf = new char[BLOCK_SIZE];
				if (dRead(address, buf) == NO_ERROR)
				{
					token = strtok(buf, ",");
					while (flag == 0)
					{
						token = strtok(NULL, ",");//files list
						cout << token <<" ";
						token = strtok(NULL, ",");
						token = strtok(NULL, ",");//now token is checking if info is end
						if (atoi(token) == 0)
							flag = 1;
						else
						{
							address = atoi(token);
							if (dRead(address, buf) == NO_ERROR)
								token = strtok(buf, ",");
							else
							{
								cout << "SomeThing went wrong! :( " << endl;
								return ERROR;
							}
						}
					}
					cout << endl;
				}
				else
				{
					cout << "SomeThing went wrong! :( Directory is not Same." << endl;
					return ERROR;
				}
				return NO_ERROR;
			}
		}
	}//now token1 has list of files

}


int fCd(char*name)
{
	char **temp = new char*[NUM_BLOCKS];
	char*token = nullptr;
	char*token1 = nullptr;
	if (disk != nullptr)
	{
		for (int i = 0; i < NUM_BLOCKS; i++)
		{
			temp[i] = new char[BLOCK_SIZE];
		}
		for (int i = 0; i < NUM_BLOCKS; i++)  //Deep copy because we are dealing with pointers.
		{
			for (int j = 0; j < BLOCK_SIZE; j++)
			{
				temp[i][j] = disk[i][j];
			}
		}
	}
	for (int i = 0; i<NUM_BLOCKS; i++)
	{
		if (disk[i] != NULL)
		{
			token = strtok(temp[i], ",");
			token1 = strtok(NULL, ",");//geting the second part if it is $ then the block is free
			if (strcmp(name, token1) == 0)//name matches
			{
				token = strtok(NULL, ",");
				if (atoi(token) == 1)//attribute matches
				{
					strcpy(CurrentDirectory, token1);
					for (int i = 0; i < NUM_BLOCKS; i++)
						delete[] temp[i];
					delete[] temp;
					return NO_ERROR;
				}
			}
		}
	}
	for (int i = 0; i < NUM_BLOCKS; i++)
		delete[] temp[i];
	delete[] temp;
	return ERROR;
}