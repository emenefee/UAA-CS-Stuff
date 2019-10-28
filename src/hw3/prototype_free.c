#include "test_functions.c"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/mman.h>

// free the memory
void free(void *ptr);

// Used to check if all the memory is free
size_t CountMemory(void *memory);
// The term usable is intended to refer to memory that will not be used by header information

int main(int argc, char *argv[])
{
    char theOption;
    char thePrinting;
    //printf("%d\n", argc);
     if (argc > 2)
     {
         theOption = *argv[2];
         thePrinting = *argv[1];
     }else{
         theOption = '1';
         thePrinting = '1';
     }
     
        void *ptr = NULL;
        int option = 1;
        if (thePrinting == '1')
        {
            option = 1;
        }
        if (theOption == '1')
        {
            ptr = makememory(1);
        }else if(theOption == '2')
        {
            ptr = makememory(2);
        }else if (theOption == '3')
        {
            ptr = makememory(3);
        }else if (theOption == '4')
        {
            ptr = makememory(4);
        }else if (theOption == '5')
        {
            ptr = makeBlockWithOneAllocation();
        }else if (theOption == '6')
        {
            ptr = makeBlockWithAllFreeBlocks();
        }

        if (theOption == '6')
        {
            CountMemory((void *)masterheader + sizeof(header_t));
        }else
        {
            printlinkedlist(ptr,option);
            CountMemory(ptr);
            free(ptr);
            printf("************\n");
            printf("************\n");
            if (masterheader != NULL)
            {
                printlinkedlist((void *)masterheader + sizeof(header_t),option);
                CountMemory((void *)masterheader + sizeof(header_t));
            }
        }
        
        
        
     
    return 0;
}




// Used to total up the memory to determine if 
// all blocks have been freed
size_t CountMemory(void *memory)
{
    header_t *temp = (header_t *)(memory - sizeof(header_t));
    int end = 0;
    long memct = sizeof(header_t);
    header_t *mstr = (header_t *)temp->mem_ptr;
    temp = mstr->next;
    while (!end)
    {
        memct = memct + temp->size;
        if (temp->next == NULL)
        {
            end = 1;
        }
        temp = temp->next;
    }

    printf("Total = %ld\n",memct);
    return (size_t)memct;
}

void free(void *ptr)
{
   
    // get the header to the memory block being passed in.
    header_t *usrmem = (header_t *)(ptr - sizeof(header_t));

     // Get the master header
    header_t *mstr = (header_t *)usrmem->mem_ptr;

    // These two pointers are used to loop through the free memory 
    // Linked list. Once a free memory header has been found with 
    // an address greater than the usrmem address, free knows that the 
    // prev pointer belongs to the free block header that sits before the 
    // block being freed
    header_t *prev = mstr;
    header_t *temp = mstr;

    // This is a temporary pointer used to hold the address
    // of the free memory block just before the block being freed
    header_t *free = NULL;

    // Loop through the Linked list to find the approprate free memory header
    while (free == NULL)
    {
        if (temp == NULL || (void *)temp > (void *)usrmem)
        {
            free = prev;
        }else
        {
            prev = temp;
            temp = temp->next;
        }
    }

    // It is possible that there is no free memory header with an address
    // lower than the memory block being released. In that case free would refer
    // to the master header.
    if (free == mstr)
    {
        usrmem->next = mstr->next;
        mstr->next = usrmem;
    }else
    {
        usrmem->next = free->next;
        free->next = usrmem;
    }

    // If all the memory has been released, call mumap
    if (CountMemory((void *)mstr + sizeof(header_t)) >= (mstr->block_size - sizeof(header_t)))
    {
        printf("muumap was called\n");
        munmap(masterheader->mem_ptr, masterheader->block_size);
        masterheader = NULL;
    }

    printf("Master header address is %ld\n", (long int)mstr);
    printf("free header address is %ld\n", (long int)free);
    return;
}




