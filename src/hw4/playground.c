/* Return 0 or 1 depending on true or false.
   Move stuff from found object into stbuf??
   profit????? */

//malloc the stat stbuff
//pass it fsobj and a stat buf....then some random ass string for path
//char* stat ... some shit... look up some things... >://///

#include <stdio.h>
#include <string.h>

//Not sure what the return type is actually going to be.
int verify_path(char* path, fsobj *fs)
{
	if(path[0] == '/') //If the first thing encountered is a '/'
	{
		char tmp[255];
		char *moreTemp;

		moreTemp = get_next_path(&path);

		if(moreTemp != path)
		{
			//tmp holds the string you need to compare
			//moreTemp holds the rest of the path
			memset(tmp, 0, 255);
			memcpy(tmp, &path[1], strlen(path)-strlen(moreTemp-1));

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
					verify_path(moreTemp, &curr_obj);
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

char *get_next_path(char *curr_path)
{
	if(curr_path[0] == '/')
	{
		char *new_path;
		new_path = memchr(&curr_path[1], '/', 254);
		if(new_path != NULL)
		{
			printf("%s\n", new_path);
			return new_path;
		}
		else
		{
			return curr_path;
		}
	}
	else
		return NULL;
}

//Said playground
int main()
{
	char a[12] = "/0mg/boo/fuk";
	char* hi = a;
	verify_path(hi);
}