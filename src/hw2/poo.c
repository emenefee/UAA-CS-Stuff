#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

// Gets the length of a given string
size_t str_len (const char *str)
{
    for (size_t len = 0;;++len) if (str[len]==0) return len;
}

void printStuff(char *msg) // using your own print for documented char *s
{
    write(1, msg, str_len(msg));
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

// Lord only knows what the actual shit is happening here....tis main
int main()
{
    int startByte = 0;
    int endByte = 0;
    int linesToPrint = 10;
    int fd = open("nanpa",O_RDONLY);

    for(int i = 0; i < linesToPrint; i++)
    {
      endByte = getBytes(fd, startByte);
      mainHelper(startByte, endByte, fd);
      startByte = endByte + 1;
    }
    return 0;
}