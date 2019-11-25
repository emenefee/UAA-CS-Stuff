// I want to make a file system that looks like

// +---/root
// |   +---/usr
// |       +--/home
// |          +---/mhakin
// |              /---test.txt
// |          +---/emenefee
// |              /---test2.txt


#include <stddef.h>
#include <sys/mman.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

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
void* map_mem(size_t size);
void printName(char name[], int length);
int parsename(char name[]);
long int getOffset(long int base_ptr, long int ptr);
long int getAddr(long int base_ptr, long int ptr);

int main(int argc, char *argv[])
{
        void *fs = map_mem(100000);

        // Create the root
        fsobj *iroot = (fsobj *)fs;

        // Set root stats
        iroot->st_size = 100000;
        iroot->magic = 42;
        iroot->st_nlink = 3;
        strcpy(iroot->name, "root\n");

        // Create usr directory
        fsobj *usrdir = (fsobj *)(fs + sizeof(fsobj));
        // Set usr directory name;
        strcpy(usrdir->name, "usr\n"); 
        usrdir->st_nlink = 3;
        
        // set up usr directory
        usrdir->st_mode = S_IFDIR;

        // Set the first item in the root to usr
        iroot->start_obj = getOffset((long int)fs, (long int)usrdir);
        iroot->next_obj = 0;

        // Set up home directory
        fsobj *homedir = (fsobj *)(fs + (2 *sizeof(fsobj)) + 1000);
        strcpy(homedir->name, "home\n");
        homedir->st_mode = S_IFDIR;
        homedir->start_obj = 0;
        homedir->next_obj = 0;
        homedir->st_nlink = 4;

        usrdir->start_obj = getOffset((long int)fs, (long int)homedir);

        // Set up the mhakin Directory
        fsobj *mhakin = (fsobj *)(fs + (3 *sizeof(fsobj)) + 1000);
        strcpy(mhakin->name, "mhakin\n");
        mhakin->st_mode = S_IFDIR;
        mhakin->start_obj = 0;
        mhakin->next_obj = 0;
        mhakin->st_nlink = 3;

        homedir->start_obj = getOffset((long int)fs, (long int)mhakin);

        // Set up the emenefee directory
        fsobj *emenefee = (fsobj *)(fs + (4 *sizeof(fsobj)) + 1000);
        strcpy(emenefee->name, "emenefee\n");
        emenefee->st_mode = S_IFDIR;
        emenefee->start_obj = 0;
        emenefee->next_obj = 0;
        emenefee->st_nlink = 3;
        mhakin->next_obj = getOffset((long int)fs, (long int)emenefee);

    
        // Add file to mhakin directory

        fsobj *txtfile1 = (fsobj *)(fs + (5 *sizeof(fsobj)) + 1000);
        strcpy(txtfile1->name, "test.txt\n");
        txtfile1->st_mode = S_IFREG;
        txtfile1->start_obj =  getOffset((long int)fs, (long int)((void *)txtfile1 + sizeof(fsobj)));
        mhakin->start_obj = getOffset((long int)fs, (long int)((void *)txtfile1));
        txtfile1->next_obj = 0;
        txtfile1->magic = 0;
        txtfile1->st_atim.tv_nsec = 0;
        txtfile1->st_mtim.tv_sec = 0;
        txtfile1->st_size = 100;

        // Add Contents of the first file


        fsobj *txtfile2 = (fsobj *)(fs + (6 *sizeof(fsobj)) + 1000);
        strcpy(txtfile2->name, "test2.txt\n");
        txtfile2->st_mode = S_IFREG;
        txtfile2->start_obj =  getOffset((long int)fs, (long int)((void *)txtfile2 + sizeof(fsobj)));
        emenefee->start_obj = getOffset((long int)fs, (long int)((void *)txtfile2));
        txtfile2->next_obj = 0;
        txtfile2->magic = 0;
        txtfile2->st_atim.tv_nsec = 0;
        txtfile2->st_mtim.tv_sec = 0;
        txtfile2->st_size = 100;

        // Add Contents of the first file




        // Below is all debugging print statements
        // ***************************************
        // ***************************************
        // ***************************************
        // ***************************************

        // print the size of the struct
        printf("Size of the struct %ld\n", sizeof(fsobj));
        // print out the file system size and magic number
        printf("File System size: %ld\n", iroot->st_size);
        printf("Magic number: %d\n", iroot->magic);
        // Print the 
        int lengthofname = parsename(usrdir->name);
        printf("Length of the name: %d\n", lengthofname);
        // FOR DEBUG
        char sbuff[lengthofname];
        for (int i = 0;i < lengthofname;i++)
        {
            sbuff[i] = usrdir->name[i];
        }
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
        return 0;
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

int parsename(char name[])
{
    int i = 0;
    while (1){
        if (name[i] == '\n')
        {
            return i;
        }
        i++;
    }
}

long int getOffset(long int base_ptr, long int ptr)
{
    return ptr - base_ptr;
}

long int getAddr(long int base_ptr, long int ptr)
{
    return base_ptr + ptr;
}