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


/* Write a function (f) that needs to change some integer to
42, and a calling function (cf): we are in a calling function
the calling function calls f
if f wants to see that change for its own var, int a;
In order to see a change done to a certain argument, pass a ptr.
some function needs to store a list of strings in mem. a str in c
is the null terminated array of chars
an array is a ptr. 
one string = char *
need a list of arrays: char**
inside a function that needs to communicate that data structure
to the outside world, the calling function (char***)
calling function does: 
char** array_of_strs
somewhere calls myfs read_dir_implem(char ***namesptr)
char *** arry_of_strs = answer;
SET UP AN ARRY OF STRS
size_t n, i, l;
n = 42 //the number of strings
array_of_strs = (char**) calloc(n, sizeof(char *));
.. Some kind of check to make sure it works
for (i = 0; i < n; i++)
{
	array_of_str[i] = (char*) calloc(L + 1, sizeof(char)) 
	strcopy(array_of_strs[i], ??? max_len L);
	array of strs[i][l] = '\0';
	&namesptr = array_of_strs;
}*/
int get_dir_list(void *fsptr, char* path, char ***namesptr, int *errnoptr)
{
	char ** array_of_strs;
	size_t n, l;
	fsobj *curr_obj = fsptr->start_obj;

	// get a count of how many things are in the directory
	while(curr_obj != NULL)
	{
		n++;
		curr_obj = curr_obj->next_obj;
	}

	//reset curr because you need to know the length of each str
	curr_obj = fsptr->start_obj;

	array_of_strs = (char **) calloc(n, sizeof(char *));

	if(array_of_strs != NULL) //if calloc was successful
	{
		for(int i=0; i < n; i++)
		{
			l = sizeof(curr_obj->name);
			array_of_strs[i] = (char *) calloc(l + 1, sizeof(char));
			if(array_of_strs[i] != NULL)
			{
				strcopy(array_of_strs[i], ???);
				array_of_strs[i][l] = '\0';
				curr_obj = curr_obj->next_obj;
			}
			else
			{
				&errnoptr = ENOMEM
				return -1;
			}
		}
		&namesptr = array_of_strs;
		return 0;
	}
	else //return errno
	{
		&errnoptr = ENOMEM;
		return -1;
	}
}

//Said playground
int main()
{
	char a[12] = "/0mg/boo/fuk";
	char* hi = a;
	verify_path(hi);
}