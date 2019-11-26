#include <string.h>
#include <stdio.h>

int main()
{
	char a[3] = "hi";
	char b[3] = "hi";

	char* c = b;

	int d = strcmp(a, c);
	printf("%d\n", d);

	char tmp[255];

	memset(tmp, 0, 3);

	char aa[12] = "/0mg/boo/fuk";

	c = memchr(&aa[1], '/', 255-1);
	printf("%s\n", c);
	if(c != NULL)
	{
		memset(tmp, 0, 255);
		memcpy(tmp, &aa[1], strlen(aa)-strlen(c-1));
	}
}