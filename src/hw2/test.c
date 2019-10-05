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

int main()
{
  int fd = open("nanpa", O_RDONLY);
  int size = sizeOfFile((char*) fd);
  int point = lseek(fd, size, SEEK_END);
  printf("%d", point);
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