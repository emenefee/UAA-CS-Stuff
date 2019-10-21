#include <stdio.h>
#include <sys/mman.h>

// typedef for the header
typedef struct{
void *mem_ptr; // the address of the whole memory block used to uniquly identify the mmap it belongs to.
size_t block_size; // size of the large block allocation
size_t size; // Size of the block given to the user including the header
void *start; // Start of the block of usable memory minus the header
void *next; // pointer to the next block of user allocated memory
} header_t;


void *prev(void *ptr);
void free(void *ptr);
int test(int argc, char *argv[]);

void *prev(void *ptr)
{
    header_t *usrmem = (header_t *)(ptr - sizeof(header_t));
    header_t *blockmem = usrmem->mem_ptr;

    header_t *prev = NULL;
    header_t *current = blockmem;
    while (1)
    {
        if (current->next == NULL)
        {
            return NULL;
        }
        prev = current;
        current = current->next;
        if (current->start == ptr)
        {
            return prev;
        }
    }
    return NULL;
}

void free(void *ptr)
{
    header_t *usermem = (header_t *)(ptr - sizeof(header_t));
    header_t *blockmem = usermem->mem_ptr;
    header_t *firstmem = blockmem->next;

    if (firstmem->next == NULL)
    {

        munmap(blockmem->mem_ptr, blockmem->block_size);

    }else if(usermem->next == NULL)
    {
        void *temp = prev(ptr);
        header_t *temp2 = (header_t *)temp;
        temp2->next = NULL;
        // set previous next to NULL
    }else
    {
        void *temp = prev(ptr);
        header_t *temp2 = (header_t *)temp;
        temp2->next = usermem->next;
        // Set previous->next = current->next
    }

}
















int test(int argc, char *argv[])
{
    
    size_t sz = sizeof(header_t); // Var to hold the size of the headder

    // block of memory plus the size of the master header
    // EM: Size of block to be created with 'malloc', 1000 + size of header
    size_t t_size = 1000 + sizeof(header_t);

    printf("The size of the memory header header_t is %lu\n", sz);


    // Pull the big block of memory that will hold the header and the various user memory allocations
    void *ptr = mmap(0, t_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // Create a Master header pointer
    header_t *freememory = (header_t *)ptr;
    // Assign values of the new master header pointer
    freememory->block_size = t_size - sizeof(header_t);
    freememory->size = freememory->block_size;
    freememory->mem_ptr = ptr;                              // Store the location of the start of the whole memory block
    freememory->start = (ptr + sizeof(header_t));
    freememory->next = NULL;





    // ****** create first user blocks of memory ****

    header_t *usr1 = freememory->start;
    // Copy the master header info
    usr1->mem_ptr = freememory->mem_ptr;
    usr1->block_size = freememory->block_size;

    // Set the size of the usr1 block
    usr1->size = 100 - sz;
    usr1->start = freememory->start + sz;
    usr1->next = NULL;

    // set the master headers next equal to this
    freememory->next = (void *)usr1;
    
    char *count = (char *)usr1->start;
    for (int i = 0; i < usr1->size;i++)
    {
        count[i] = 'a';
    }

    for (int f = 0; f < usr1->size;f++)
    {
        printf("%c\t", count[f]);
    }

    printf("The address of usr1: %p\n", (void *)usr1);
    printf("The address of freememory->next: %p\n", freememory->next);
    printf("End First user block of memory\n\n");

    // ***** End first user block of memory 









    // ****** create second user block of memory ****
    printf("Start The usr2 allocation\n");
    header_t *usr2 = freememory->start + sizeof(header_t) + usr1->size;
    // Copy the master header info
    usr2->mem_ptr = freememory->mem_ptr;
    usr2->block_size = freememory->block_size;

    // Set the size of the usr1 block
    usr2->size = 100 - sz;
    usr2->start = (void *)usr2 + sz;
    usr2->next = NULL;

    // set the master headers next equal to this
    usr1->next = (void *)usr2;
    
    char *count2 = (char *)usr2->start;
    for (int o = 0; o < usr2->size;o++)
    {
        count2[o] = 'b';
    }

    for (int k = 0; k < usr2->size;k++)
    {
        printf("%c\t", count2[k]);
    }

    printf("The address of usr2: %p\n", (void *)usr2);
    printf("The address of usr1->next: %p\n", usr1->next);
    printf("End second user block of memory\n\n");

    // ***** End second user block of memory 



    




// ****** create third user block of memory ****
    printf("Start The usr3 allocation\n");
    header_t *usr3 = freememory->start + sizeof(header_t) + usr1->size + sizeof(header_t) + usr2->size;
    // Copy the master header info
    usr3->mem_ptr = freememory->mem_ptr;
    usr3->block_size = freememory->block_size;

    // Set the size of the usr1 block
    usr3->size = 100 - sz;
    usr3->start = (void *)usr3 + sz;
    usr3->next = NULL;

    // set the master headers next equal to this
    usr2->next = (void *)usr3;
    
    char *count3 = (char *)usr3->start;
    for (int m = 0; m < usr3->size;m++)
    {
        count3[m] = 'c';
    }

    for (int b = 0; b < usr3->size;b++)
    {
        printf("%c\t", count3[b]);
    }

    printf("The address of usr3: %p\n", (void *)usr3);
    printf("The address of usr2->next: %p\n", usr2->next);
    printf("End third user block of memory\n\n");

    // ***** End third user block of memory 



    void *theprev = prev(usr3->start);


    printf("The prev to usr3 is %p\n", theprev);

    void *testptr = usr2->start;

    free(testptr);

    printf("The next to usr1 is %p\n", usr1->next);

    // Testing to see if I can free memory from a new pointer.
    munmap(ptr, t_size + sizeof(header_t));

    
    return 0;
}








int main(int argc, char *argv[])
{

    test(argc, argv);

    
    return 0;


}