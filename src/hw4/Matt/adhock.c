// I want to make a file system that looks like

// +---/
// |   /---.
// |   /---..
// |   +---/usr
// |       /---.
// |       /---..
// |       +--/home
// |          /---.
// |          /---..
// |          +---/mhakin
// |              /---.
// |              /---..
// |              /---test.txt
// |              /---my dog.jpg
// |              /---accounts.pdb
// |          +---/emenefee
// |              /---.
// |              /---..
// |              /---test2.txt

#include "prototypes.c"

int __myfs_getattr_implem(void *fsptr, size_t fssize, int *errnoptr,
                          uid_t uid, gid_t gid,
                          const char *path, struct stat *stbuf) {

    char *search_obj = getlastiteminpath(path);
    char *realpath = copystring(path);
    long int top = (long int)fsptr;
    //long int temptop = (long int)fsptr;

    fsobj *root = (fsobj *)fsptr;

    // Check to see if the header is formated
    if (root->magic != 42 || root->name[0] != '/'){
        return -1;
    }else{
        root = (fsobj *)getAddr((long int)root, root->start_obj);
    }
    
    int result = followpath((void *)root, realpath, search_obj, top, errnoptr, stbuf, uid, gid);
    
    if (result != 1){
        free(realpath);
        return -1;
    }else{
        free(realpath);
        return 1; }
}

int main(int argc, char *argv[])
{
 // const char *testpath = "/usr/home/mhakin/accounts.pdb\n";
 // const char *testpath = "/usr/home/emenefee/test2.txt\n";
 // const char *testpath = "/usr/home/emenefee\n";
 // const char *testpath = "/usr/home/mhakin/my dog.jpg\n";
 // const char *testpath = "/usr/home/mhakin/test.txt\n";
 // const char *testpath = "/usr/home/mhakin\n";
 // const char *testpath = "/usr/home\n";
 // const char *testpath = "/usr/homes\n";
 // const char *testpath = "/usr\n";
 //char *testpath = argv[1];
char *testpath;
    if (argc > 1){
        char *temp = argv[1];
        int chlen = strlen(temp) + 1;
        testpath = (char *)malloc(chlen);
        for (int i = 0;i<chlen;i++){
            testpath[i] = temp[i]; }
        testpath[chlen - 1] = '\n';
    }else{ return -1; }
    

    // Set up adhock file system
    fsobj *fs = adhock();

    int *errormsg = (int *)malloc(sizeof(int));
    
    struct stat stbuf;

    int result =__myfs_getattr_implem((void *)fs, 100000, errormsg, 42, 42, testpath, &stbuf);
    
    if (result == 1){
        printf("Search Path %s", testpath);
        printf("stbuff gid %d\n", stbuf.st_gid);
        printf("stbuff uid %d\n", stbuf.st_uid);
        printf("stbuff st_mode %d\n", stbuf.st_mode);
        printf("stbuff st_size %ld\n", stbuf.st_size);
        printf("stbuff st_nlink %ld\n", stbuf.st_nlink);
    }else{
        printf("object was not found\n");
    }
    return 0;
}








