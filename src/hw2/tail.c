/*  Elysha Menefee & Matt Hakin  */
/* CSCE A321 - Operating Systems */
/*         Date: 10/7/2019       */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>

// sizeOfFile()
// Purpose: Obtain size of file, if file exists
// Inputs: char array that holds the address of a file
// Outputs: 0 if failed, size of file if success (in bytes)
int sizeOfFile(char file[]);

// strCompare()
// Purpose: Check if two strings are the same
// Input: Given char array and char array to check
// Output: True if they match, false otherwise
bool strCompare(char argStr[], char checkStr[]);

// openFile()
// Purpose: Check to see if a file has the ability to be open
// Uses: open() with inputs of the name of the file as well as a read only parameter
// Inputs: char array that holds the address of a file
// Outputs: a file descriptor depicting the status of a file(
//			-1 if failed, non-negative if success)
int openFile(char fileName[]);

// closeFile()
// Purpose: Close file
// Uses:
// Inputs: File descriptor from open file
// Outputs: 0 on success, -1 on fail
int closeFile(int fd);

// strToInt()
// Purpose: Convert a string to an integer
// Input: a char array that is hopefully an int
// Output: the int gained from input
int strToInt(char argStr[]);

// str_len()
// Purpose: Get the length of a given array
// Input: An array
// Output: length of array
size_t str_len(const char *str);

void readFile(char *buffer, int size, int fd)
{
	ssize_t bytesRead;
	bytesRead = read(fd, buffer, size);
	//TODO: Some error handling
}

int tailSearch2(char *buffer, int linesToPrint, int sizeOfFile)
{
	int newLines = 0, byteAddr = 0;
	char *val = "\n";
	char currChar;

	for(int i = sizeOfFile; i >= 0; i--)
	{
		currChar = buffer[i];

		if((newLines < (linesToPrint + 1)) && (currChar == *val))
		{
			newLines++;
		}
		else if((linesToPrint + 1) > newLines)
		{
			byteAddr = i;
		}
	}
	return byteAddr;
}

void printTail(char *buffer, int linesToPrint, int sizeOfFile)
{
	int startAddr = 0;
	startAddr = tailSearch2(buffer, linesToPrint, sizeOfFile);

	for(int i = startAddr; i <= sizeOfFile; i++)
	{
		write(1, &buffer[i], 1);
	}
}

// -----------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int linesToPrint = 10, fd = 0, size = 0;
	bool numFlag = false, fileFlag = false;
	char *file, *buffer;
	char numStr[] = "-n";

	for(int i = 1; i < argc; i++)
	{
		char *argStr = argv[i];
		if(strCompare(argStr, numStr))
		{
			numFlag = true;
			i++;
			argStr = argv[i];
			linesToPrint = strToInt(argStr);
		}
		else if((fd = openFile(argv[i])) > 0)
		{
			fileFlag = true;
			file = argv[i];
		}
	}

	if(fileFlag)
	{
		size = sizeOfFile(file);
		buffer = (char *) malloc(size);
		readFile(buffer, size, fd);

		printTail(buffer, linesToPrint, size);
		free(buffer);
		closeFile(fd);
	}

	return 0;
}
// -----------------------------------------------------------------------

int sizeOfFile(char file[])
{
	struct stat fileStat;

	if(stat(file, &fileStat) != 0)
	{
		return 0;
	}
	return fileStat.st_size;
}

bool strCompare(char argStr[], char checkStr[])
{
	int i = 0;
	bool valueStatus = false;

	for(i; !(argStr[i] == '\0' || checkStr[i] == '\0'); i++)
	{
		if(argStr[i] == checkStr[i])
			valueStatus = true;
		else
			valueStatus = false;
	}

	if((argStr[i] == '\0' && checkStr[i] == '\0') && valueStatus)
		return true;
	else	
		return false;
}

int openFile(char fileName[])
{
	int fd;

	fd = open(fileName, O_RDONLY);
	if(fd == -1)
	{
		perror("File Error: ");
		exit(1);
	}

	return fd;
}

int closeFile(int fd)
{
	if(close(fd) < 0)
	{
		perror("c1");
		exit(1);
	}
	return close(fd);
}

int strToInt(char argStr[])
{
	int len = str_len(argStr), num = 0;
	for(int i = 0; i < len; i++)
	{
		num = num * 10 + (argStr[i] - '0');
	}
	return num;
}

size_t str_len(const char *str)
{
    for (size_t len = 0;;++len) if (str[len]==0) return len;
}