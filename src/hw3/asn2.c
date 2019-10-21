#include <stdio.h>
#include <sys/mman.h>

// typedef for the header
typedef struct{
void *mem_ptr; // the address of the whole memory block used to uniquly identify the mmap it belongs to.
int size;
void *start;
} header_t;

int main(int argc, char *argv[])
{
    // I just wanted to see how big a header_t object is
    size_t sz = sizeof(header_t);
    printf("The size of the memory header header_t is %lu\n", sz);

    // fill a variable with the size of the header
    size_t t_size = 90 + sizeof(header_t);

    // Pull the big block of memory that will hold the header and the user memory
    void *ptr = mmap(0, t_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // Create a header pointer
    header_t *freememory = (header_t *)ptr;
    // Assign values the the new header pointer object
    freememory->size = t_size - sizeof(header_t);
    freememory->mem_ptr = ptr;
    freememory->start = (ptr + sizeof(header_t));

    // Actually pull the block of memory the user will be allowed to use
    void* ptr2 = ptr + sizeof(header_t);
    
    // Create a char pointer to the user memory
    char* msg = (char *)ptr2;

    // Write something to the memory the user has
    msg[89] = 'a';

    // Print the starting address of the whole block
    printf("The address of the start of the whole block is %p\n", ptr);
    // Write the starting address to the memory actually given to the user
    printf("The start of the block minus the header is %p\n", ptr2);

    // Print something written to what would be the block of memory given to the user
    printf("Wrote a char to the last element of the allocated user memory block it is: '%c'\n", msg[89]);

    // Assign the memory header to a new pointer to see if we can get the header back
    header_t *getshit = (header_t *)(ptr2 - sizeof(header_t));

    printf("Size of the memory block minus the header %d\n", getshit->size);
    printf("Assigned the whole block to a new header_t called getshit. the original pointer matches: %p\n",getshit->mem_ptr);
    printf("getshit's start of useable memory: %p\n",getshit->start); 

    // Testing to see if I can free memory from a new pointer.
    munmap(getshit->mem_ptr, getshit->size + sizeof(header_t));


    
    return 0;


}