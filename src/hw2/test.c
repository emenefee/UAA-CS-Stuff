#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// sizeOfFile()
// Purpose: Obtain size of file, if file exists
// Inputs: char array that holds the address of a file
// Outputs: 0 if failed, size of file if success (in bytes)
int sizeOfFile(char file[]);

void readFile(char *buffer, int size, int fd)
{
  ssize_t bytesRead;
  bytesRead = read(fd, buffer, size);
  //TODO: Some error handling
}

int tailSearch2(char *buffer, int linesToPrint, int size)
{
  int newLines = 0, byteAddr = 0;
  char *val = "\n";
  char tempBuff;

  for(int i = size; i >= 0; i--)
  {
    tempBuff = buffer[i];
    if((newLines < (linesToPrint + 1)) && (tempBuff == val))
    {
      newLines++;
    }
    else if(newLines == (linesToPrint + 1))
    {
      byteAddr = (int) buffer[i];
      return byteAddr;
    }
  }
}

void printTail(char *buffer, int linesToPrint, int sizeOfFile)
{
  int startAddr = 0;
  startAddr = tailSearch2(buffer, linesToPrint, size);

  for(int i = startAddr; i <= sizeOfFile; i++)
  {

    write(1, buffer[i], 1);
  }
}

// find x amount of lines to print
// get that last byte address (???)
// size - byteAddress
// allocate new buffer ^
// print ^

int main()
{
  int fd = open("nanpa", O_RDONLY);
  int size = sizeOfFile("nanpa");

  char *buffer;
  buffer = (char *) malloc(size);
  readFile(buffer, size, fd);

  printTail(buffer, 10, size);

  free(buffer);
  close(fd);
  return 0;
}

int sizeOfFile(char file[])
{
  struct stat fileStat;

  if(stat(file, &fileStat) != 0)
  {
    return 0;
  }
  return fileStat.st_size;
}