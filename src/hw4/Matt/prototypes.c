#include <stdint.h>
#include <stddef.h>
#include <sys/mman.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>


typedef struct {
        long int start_obj;     // if file, offset of current object to start of file || if dir,
                                //    points to begining of ll
        long int next_obj;      // ^
        char name[255];         // Name of file/dir
        uint8_t magic;
        uid_t st_uid;           // User ID of owner     - uint
        gid_t st_gid;           // Group ID of owner    - uint
        mode_t st_mode;         // File (S_IFREG) || Directory (S_IFDIR)
        nlink_t st_nlink;       // (as many as there are subdirectories (not files) for directories(including . and ..),1 for files)
        off_t st_size;          // (supported only for files, where it is the real file size)

        struct timespec st_atim; // Last access time
        struct timespec st_mtim; // Last modification time
    } fsobj;


// Function prototypes
int comparePath(char *org, char *test);
void* map_mem(size_t size);
void printName(char name[], int length);
int parsename(char name[]);
long int getOffset(long int base_ptr, long int ptr);
long int getAddr(long int base_ptr, long int ptr);
int __myfs_getattr_implem(void *fsptr, size_t fssize, int *errnoptr, uid_t uid, gid_t gid, const char *path, struct stat *stbuf);
int findNextToken(char *path);
char *truncatePath(char *path);
int parseAndCompare(char *testpath, fsobj *fs, long int top, char *searchitem, int *errormsg);
int followpath(fsobj *fs, char *path, char *searchItem, long int top, int *errormsg, struct stat *stbuf, uid_t uid, gid_t gid);
char *copystring(const char *copy);
int countpathseparators(char *path);
void print_current_object(long int top,long int temptop, fsobj *fs, char *testpath);
char *getlastiteminpath(const char *path);
fsobj *adhock();
void loadstat(struct stat *stbuf, fsobj *entity, uid_t uid, gid_t gid);

int followpath(fsobj *fs, char *path, char *searchItem, long int top, int *errormsg, struct stat *stbuf, uid_t uid, gid_t gid)
{
    int lennn = strlen(path);
    char *temppath = (char *)malloc(lennn);
        if (countpathseparators(path) == 0){
            for (int i = 1;i < lennn;i++){
                temppath[i - 1] = path[i];
            }   
        }

    if (strcmp(fs->name,temppath) == 0 ){
        loadstat(stbuf, fs, uid, gid);
        return 1;
    }

    if (fs->name == searchItem && truncatePath(path) == NULL){
        // do stuff
        free(temppath);
        loadstat(stbuf, fs, uid, gid);
        return 1;
    }
    int success = parseAndCompare(path, fs, top, searchItem, errormsg);

    if (success != 1) {
        if (fs->next_obj != 0 && success != 100){
            fs = (fsobj *)(getAddr(top, (long int)fs->next_obj));
            return followpath(fs, path, searchItem, top, errormsg, stbuf, uid, gid);
            free(temppath);
        }else if (success == 100 && fs->next_obj != 0){   
            fs = (fsobj *)(getAddr(top, (long int)fs->next_obj));
            free(temppath);
            return followpath(fs, path, searchItem, top, errormsg, stbuf, uid, gid);
        }else{
            // have reached the end with nothing
            free(temppath);
            return -1;
        }
    // move down
    }else{
       if (success == 100){
           //printf("100");
           free(temppath);
           return 1;
       }
       if (fs->start_obj != 0){
            path = truncatePath(path);
            fs = (fsobj *)(getAddr(top, (long int)fs->start_obj));
            free(temppath);
            return followpath(fs, path, searchItem, top, errormsg, stbuf, uid, gid);
       }else{
            if (fs->next_obj != 0){   
                fs = (fsobj *)(getAddr(top, (long int)fs->next_obj));
                free(temppath);
                return followpath(fs, path, searchItem, top, errormsg, stbuf, uid, gid);
            }else{
                free(temppath);
                return -1;
            } 
       }  
    }
    return 0;
}


// Loads info int stbuff
void loadstat(struct stat *stbuf, fsobj *entity, uid_t uid, gid_t gid)
{
    stbuf->st_size = entity->st_size;
    stbuf->st_uid = uid;
    stbuf->st_gid = gid;
    stbuf->st_mode  = entity->st_mode;
    stbuf->st_nlink = entity->st_nlink;
}


//  gets the last item in the path
char *getlastiteminpath(const char *path)
{
    int pathlen = strlen(path);
    int currlast = 0;
    for (int i = 0;i<pathlen;i++)
    {
        if (path[i] == '/')
        {
            currlast = i;
        }
    }

    if (currlast > 0)
    {
        int newlen = pathlen - currlast + 1;
        char *cpy = (char *)malloc(newlen);
        int internal = 0;
        for (int i = currlast+1;i < pathlen;i++)
        {
            cpy[internal] = path[i];
            internal++;
        }
        return cpy;
    }
    return NULL;
}


void print_current_object(long int top,long int temptop, fsobj *fs, char *testpath)
{
    if ((long int)fs == temptop) {// failed{
        printf("Not Found\n");
        printf("Path remains %s\n", testpath);
        printf("Current object remains: %s\n",fs->name);
    }else{
        testpath = truncatePath(testpath);
        temptop = (long int)fs;
        printf("Path: %s\n", testpath);
        printf("Current object: %s\n",fs->name);
    }
}


int countpathseparators(char *path)
{
    int lenn = strlen(path);
    int ct = 0;
    for (int i = 1 ;i<lenn;i++){
        if (path[i] == '/'){
            ct++;
        }
    }
return ct;
}

char *copystring(const char *copy){
    int lenn = strlen(copy);
    char *cpy = (char *)malloc(lenn);
    for (int i = 0;i<lenn;i++){
       cpy[i] = copy[i];
   }

   return cpy;
}

fsobj *adhock()
{
        void *fs = map_mem(100000);

        // Create the root
        fsobj *iroot = (fsobj *)fs;

        // Set root stats
        iroot->st_size = 100000;
        iroot->magic = 42;
        iroot->st_nlink = 3;
        strcpy(iroot->name, "/\n");

        // Create usr directory
        fsobj *usrdir = (fsobj *)(fs + sizeof(fsobj));
        // Set usr directory name;
        strcpy(usrdir->name, "usr\n"); 
        usrdir->st_nlink = 3;
        
        // set up usr directory
        usrdir->st_mode = S_IFDIR;

        // Set up home directory
        fsobj *homedir = (fsobj *)(fs + (2 *sizeof(fsobj)) + 1000);
        strcpy(homedir->name, "home\n");
        homedir->st_mode = S_IFDIR;
        homedir->start_obj = 0;
        homedir->next_obj = 0;
        homedir->st_nlink = 4;

        // Set up the mhakin Directory
        fsobj *mhakin = (fsobj *)(fs + (3 *sizeof(fsobj)) + 1000);
        strcpy(mhakin->name, "mhakin\n");
        mhakin->st_mode = S_IFDIR;
        mhakin->start_obj = 0;
        mhakin->next_obj = 0;
        mhakin->st_nlink = 5;


        // Set up the emenefee directory
        fsobj *emenefee = (fsobj *)(fs + (4 *sizeof(fsobj)) + 1000);
        strcpy(emenefee->name, "emenefee\n");
        emenefee->st_mode = S_IFDIR;
        emenefee->start_obj = 0;
        emenefee->next_obj = 0;
        emenefee->st_nlink = 3;
        
        // Add file to mhakin directory

        fsobj *txtfile1 = (fsobj *)(fs + (5 *sizeof(fsobj)) + 1000);
        strcpy(txtfile1->name, "test.txt\n");
        txtfile1->st_mode = S_IFREG;
        txtfile1->start_obj =  getOffset((long int)fs, (long int)((void *)txtfile1 + sizeof(fsobj)));
        
        txtfile1->next_obj = 0;
        txtfile1->magic = 0;
        txtfile1->st_atim.tv_nsec = 123456789;
        txtfile1->st_mtim.tv_sec = 987654321;
        txtfile1->st_size = 2900;

        // Add Contents of the first file


        fsobj *txtfile2 = (fsobj *)(fs + (6 *sizeof(fsobj)) + 1000);
        strcpy(txtfile2->name, "test2.txt\n");
        txtfile2->st_mode = S_IFREG;
        txtfile2->start_obj =  getOffset((long int)fs, (long int)((void *)txtfile2 + sizeof(fsobj)));
        
        txtfile2->next_obj = 0;
        txtfile2->magic = 0;
        txtfile2->st_atim.tv_nsec = 0;
        txtfile2->st_mtim.tv_sec = 0;
        txtfile2->st_size = 7090;

        fsobj *imagefile = (fsobj *)(fs + (7 *sizeof(fsobj)) + 1000);
        strcpy(imagefile->name, "my dog.jpg\n");
        imagefile->st_mode = S_IFREG;
        imagefile->start_obj =  getOffset((long int)fs, (long int)((void *)imagefile + sizeof(fsobj)));
        
        imagefile->next_obj = 0;
        imagefile->magic = 0;
        imagefile->st_atim.tv_nsec = 15230000;
        imagefile->st_mtim.tv_sec = 999999999;
        imagefile->st_size = 100;


        fsobj *acctDoc = (fsobj *)(fs + (9 *sizeof(fsobj)) + 1000);
        strcpy(acctDoc->name, "accounts.pdb\n");
        acctDoc->st_mode = S_IFREG;
        acctDoc->start_obj =  getOffset((long int)fs, (long int)((void *)acctDoc + sizeof(fsobj)));
        
        acctDoc->next_obj = 0;
        acctDoc->magic = 0;
        acctDoc->st_atim.tv_nsec = 3333333333;
        acctDoc->st_mtim.tv_sec = 4444444444;
        acctDoc->st_size = 6969;



        // Set the first item in the root to usr
        iroot->start_obj = getOffset((long int)fs, (long int)usrdir);
        iroot->next_obj = 0;

        usrdir->start_obj = getOffset((long int)fs, (long int)homedir);

        homedir->start_obj = getOffset((long int)fs, (long int)mhakin);

        mhakin->next_obj = getOffset((long int)fs, (long int)emenefee);

        mhakin->start_obj = getOffset((long int)fs, (long int)((void *)txtfile1));

        emenefee->start_obj = getOffset((long int)fs, (long int)((void *)txtfile2));

        txtfile1->next_obj = getOffset((long int)fs, (long int)imagefile);

        imagefile->next_obj = getOffset((long int)fs, (long int)acctDoc);

        // Add Contents of the first file




        // Below is all debugging print statements
        // ***************************************
        // ***************************************
        // ***************************************
        // ***************************************

        // print the size of the struct
        //printf("Size of the struct %ld\n", sizeof(fsobj));
        // print out the file system size and magic number
        //printf("File System size: %ld\n", iroot->st_size);
        //printf("Magic number: %d\n", iroot->magic);
        // Print the 
        //int lengthofname = parsename(usrdir->name);
        //printf("Length of the name: %d\n", lengthofname);
        // FOR DEBUG
        //char sbuff[lengthofname];
        //for (int i = 0;i < lengthofname;i++)
        //{
        //    sbuff[i] = usrdir->name[i];
        //}

        /*
        // FOR DEBUG
        printf("%s\n", sbuff);



        printf("The root directory's starting objects offset\n");
        printf("%ld\n", iroot->start_obj);
        printf("%ld\n", getOffset((long int)fs, (long int)usrdir));


        printf("The usr directory's starting objects offset\n");
        printf("%ld\n", usrdir->start_obj);
        printf("%ld\n", getOffset((long int)fs, (long int)homedir));

        printf("The home directory's starting objects offset\n");
        printf("%ld\n", homedir->start_obj);
        printf("%ld\n", getOffset((long int)fs, (long int)mhakin));

        printf("The mhakin directory's next object offset\n");
        printf("%ld\n", mhakin->next_obj);
        printf("%ld\n", getOffset((long int)fs, (long int)emenefee));

        printf("The mhakin file\n");
        printf("%ld\n", mhakin->start_obj);
        printf("%ld\n", getOffset((long int)fs, (long int)txtfile1));

        printf("The emenefee file\n");
        printf("%ld\n", emenefee->start_obj);
        printf("%ld\n", getOffset((long int)fs, (long int)txtfile2));

        // FOR DEBUG
        fsobj *temp = (fsobj *)getAddr((long int)fs, (long int)emenefee->start_obj);
        printf("%s\n", temp->name);

        char ssbuff[255] = "matt\n";

        char *ssbuf2 = "matt\n";

        int poo = strcmp(ssbuff, ssbuf2);

        printf("String Compare %d\n", poo);


        char newbuff[255];

        strcpy(newbuff, "Holy Shit/cowpatty\n");


        void *answer = memchr(newbuff,'/',10);

        
        */


        return iroot;
}


    // helper function to pull memory from the heap
void* map_mem(size_t size)
{
	void* ptr = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	return ptr;
}

void printName(char name[], int length)
{
    for (int i=0;i<length;i++)
    {
        printf("%c", name[i]);
    }
    printf("\n");
}

long int getOffset(long int base_ptr, long int ptr)
{
    return ptr - base_ptr;
}

long int getAddr(long int base_ptr, long int ptr)
{
    return base_ptr + ptr;
}

int comparePath(char *org, char *test)
{
    
    int answer = strcmp(org, test);
    char output[10];
    if (answer == 0)
    {
        strcpy(output, "Match\n");
        //printf("%s\n", output);
        return 1;
    }else
    {
        strcpy(output, "No Match\n");
        //printf("%s\n", output);
        return -1;
    }

}

int findNextToken(char *path)
{
    int position = 0;
    //int length = strlen(path);
    //printf("String Length = %d\n", length);
    int ct =1;
    while (1)
    {
        if (path[ct] == '/')
        {
            position = ct;
            break;
        }else
        {
            ct++;
        }
        if (ct >= 255)
        {
            position = -1;
            break;
        }
    }

    return position;
}

char *truncatePath(char *path)
{
    int trim = findNextToken(path);
     
     if (trim == -1){
         return NULL;
     }
    int totallength = strlen(path);
    //int newLength = totallength - trim;

    char *newpath = malloc(totallength);

    //char *newpath[newLength];

    for (int i = trim;i<totallength;i++){
        newpath[i-trim] = path[i];
    }
    return newpath;
}

int parsename(char name[])
{
    int i = 0;
    while (1){
        if (name[i] == '\n'){
            return i;
        }
        i++;
    }
}

// This function compares the directory being search for and the curr_object
int parseAndCompare(char *testpath, fsobj *fs, long int top, char *searchitem, int *errormsg)
{
    // find Token '/' Location
    int tokenlocation = findNextToken(testpath);
    
    if (tokenlocation == -1)
    {
        int len = 0;
        while (len < 100){
            if (testpath[len] == '\n'){
                return 100;
            }
            len++;
        }
    }

    // Create Buffer For the token being searched for
    char *curr_token = (char*)malloc(tokenlocation);

    for (int i = 1;i<tokenlocation;i++){
        curr_token[i-1] = testpath[i];
    }
    int temp = tokenlocation - 1;
    curr_token[temp] = '\n';

    // Create a name buffer to hold the name proerty of the curr node
    char *nameBuff = (char*)malloc(tokenlocation);

    for (int i = 0;i<tokenlocation ;i++){
        nameBuff[i] = fs->name[i];
    }

    int success = comparePath(nameBuff,curr_token);

    if (success == 1){ return 1; }else { return -1;}
    
    return 0;
}