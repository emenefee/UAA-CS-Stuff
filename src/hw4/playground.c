/* Return 0 or 1 depending on true or false.
   Move stuff from found object into stbuf??
   profit????? */

//malloc the stat stbuff
//pass it fsobj and a stat buf....then some random ass string for path
//char* stat ... some shit... look up some things... >://///

#include <stdio.h>
#include <string.h>

//Not sure what the return type is actually going to be.
int verify_path(char* path, fsobj *fs, struct stat *stbuf)
{
	if(path[0] == '/') //If the first thing encountered is a '/'
	{
		// /* Look through the path until the next slash */
		// // *** i is initialized to 1 to avoid the leading slash
		// for(int i = 1; path[i] != '/'; i++)
		// {
		// 	count++; //This is to figure out the array length
		// }

		// char a[count]; // Arbitrary size char array
		// a[count-1] = '\0'; // Null terminate the array
		// char* ah = a; // char* pointing to starting address of a
		
		// for(int j = 0; path[j+1] != '/'; j++)
		// {
		// 	a[j] = (char)path[j+1];
		// }
		// ah = a;

		// fsobj *tmp = cmp_to_ll(ah, curr);
		// if(tmp != curr)
		// {
		// 	verify_path(&path[count], tmp, struct stat *stbuf);
		// }
		// else
		// 	return 0;
		char tmp[255];
		char *moreTemp;

		moreTemp = memchr(&path[1], '/', 255-1);
		if(moreTemp != NULL)
		{
			memset(tmp, 0, 255);
			memcpy(tmp, &path[1], strlen(path)-strlen(moreTemp-1));
			//tmp holds the string you need to compare
			//moreTemp holds the rest of the path
			fsobj *curr_obj = fs;
			while(1)
			{
				if(curr_obj->name == tmp)
				{
					if(moreTemp[0] == '\0')
					{
						/* if you're looking at the last thing, Do happy stuff 
					assign all the buffer stuff and return a 1*/
						return 1;
					}
					curr_obj = curr_obj->start_obj;
					verify_path(moreTemp, &curr_obj, &stbuf);
				}
				else if(curr_obj->next_obj != 0)
				{
					curr_obj = curr_obj->next_obj;
				}
				else
				{
					return -1
				}
			}
		}
	}
	else if(path[0] == '\0') //If you're at the end of the path.. You've probably found all the things
	{
		return 1;
	}
	else // If the first thing encountered isn't a /
	{
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