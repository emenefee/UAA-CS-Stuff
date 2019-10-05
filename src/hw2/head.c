/*  Elysha Menefee & Matt Hakin  */
/* CSCE A321 - Operating Systems */
/*         Date: 10/7/2019       */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

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

// getBytes()
// Purpose: Return the number of bytes in each line of the file
// Inputs: int fd from a file, int start amount of bytes (where you 
// 		   want search to start)
// Outputs: Returns number of bytes found in a line up until the newline 
// 			char
int getBytes(int fd, int start);

// str_len()
// Purpose: Get the length of a given array
// Input: An array
// Output: length of array
size_t str_len(const char *str);

// printStuff()
// Purpose: Print a char array to display
// Input: A char array that you want to display
// Output: NA
void printStuff(char *msg);

// strCompare()
// Purpose: Check if two strings are the same
// Input: Given char array and char array to check
// Output: True if they match, false otherwise
bool strCompare(char argStr[], char checkStr[]);

int strToInt(char argStr[]);

// mainHelper()
// Purpose: Calls other functions... Frees up space in main
// Input: int start bytes, int end bytes, int fd- file descriptor
// Output: NA
void mainHelper(int startByte, int endByte, int fd);

//------------------------------------------------------------------
// Best knowledge, will work for everything starting with ./head
int main(int argc, char *argv[])
{
	int linesToPrint = 10, len = 0, fd = 0;
	bool numFlag = false, echoFlag = false, fileFlag = false;
	char numStr[] = "-n";

	for(int i = 1; i < argc; i++) // for the arguments in argv OR while ( i < argc )
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
		}
	}

	if(fileFlag)
	{
		int startByte = 0;
	    int endByte = 0;

	    for(int i = 0; i < linesToPrint; i++)
	    {
	      endByte = getBytes(fd, startByte);
	      mainHelper(startByte, endByte, fd);
	      startByte = endByte + 1;
	    }
	}
	closeFile(fd);
	return 0;
}

// ---------------------------------------------------------------------

int openFile(char fileName[])
{
	int fd;

	fd = open(fileName, O_RDONLY);
	if(fd == -1)
	{
		perror("c1");
		exit(1);
	}

	return fd;
}

int closeFile(int fd)
{
	if(close(fd) < 0)
	{
		perror("c1");
		exit(1); // TODO: 3rd level
	}
	return close(fd);
}

int getBytes(int fd, int start)
{
  int byteCount = start;
  int readItem = 0;
  char buffer[1];
  char *val2 = "\n";
  char *val3 = "\0";

  while(1)
  {
    lseek(fd, byteCount, SEEK_SET);
    read(fd, buffer, 1); // read returns the NEXT byte
    char *val = buffer;
    if(*val == *val2 || *val == *val3 || *val == EOF)
    {
      return byteCount;
    }
    byteCount++;
  }
}

size_t str_len(const char *str)
{
    for (size_t len = 0;;++len) if (str[len]==0) return len;
}

void printStuff(char *msg)
{
    write(1, msg, str_len(msg));
}

void mainHelper(int startByte, int endByte, int fd)
{
  int totalByte = endByte - startByte;

  lseek(fd, startByte, SEEK_SET);
  char buffer[totalByte];
  read(fd, buffer, totalByte);
  char *output = buffer;

  printStuff(output);
  printStuff("\n");
}

// TODO: Document these OR GET THE FUCK RID OF THEM

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

int strToInt(char argStr[])
{
	int len = str_len(argStr), num = 0;
	for(int i = 0; i < len; i++)
	{
		num = num * 10 + (argStr[i] - '0');
	}
	return num;
}