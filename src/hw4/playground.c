/* Return 0 or 1 depending on true or false.
   Move stuff from found object into stbuf??
   profit????? */

//malloc the stat stbuff
//pass it fsobj and a stat buf....then some random ass string for path
//char* stat ... some shit... look up some things... >://///

#include <stdio.h>
#include <string.h>

//Not sure what the return type is actually going to be.
int verify_path(char* path, fsobj *curr, struct stat *stbuf)
{
	int count;
	if(path[0] == '/') //If the first thing encountered is a '/'
	{
		/* Look through the path until the next slash */
		// *** i is initialized to 1 to avoid the leading slash
		for(int i = 1; path[i] != '/'; i++)
		{
			count++; //This is to figure out the array length
		}

		char a[count]; // Arbitrary size char array
		a[count-1] = '\0'; // Null terminate the array
		char* ah = a; // char* pointing to starting address of a
		
		int j; // use to populate a & ah = a to shove into the recusive step
		for(j = 0; path[j+1] != '/'; j++)
		{
			a[j] = (char)path[j+1];
		}
		ah = a;

		if(cmp_to_ll(ah, curr))
		{
			for(int i = 0; path[count] != '\0'; i++)
			{
				count += 1;
				a[0] = 
			}
		}
		return 0;
	}
	else if(path[0] == '\0') //If you're at the end of the path.. You've probably found all the things
	{
		return 1;
	}
	else // If the first thing encountered isn't a /
	{
		/*TODO: check root directory*/
		//else
		printf("%d\n", -1);
		return 0;
	}
}

int cmp_to_ll(char* check, fsobj *curr)
{
	/* TODO: Traverse ll to find check? */ 
	while(ll != NULL)
	{

	}
	for(int i=0; ll == NULL; i++)
	{
		if(strcmp(a, ll) == 0)
		{
			return 1;
		}
		else
			return 0;
	}
}

//Said playground
int main()
{
	char a[12] = "/0mg/boo/fuk";
	char* hi = a;
	verify_path(hi);
}