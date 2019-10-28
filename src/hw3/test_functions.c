// Prtotypes

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/mman.h>

typedef struct{
    void *mem_ptr;      // the address of the whole memory block used to uniquly identify the mmap it belongs to.    -- All will be the same for all
    size_t block_size;  // size of the large block allocation                                                        -- All will be the same for all
    size_t size;        // Size of the block given to the user not including the header                              -- User Memory 
    void *start;        // Start of the block of usable memory minus the header                                      -- User Memory
    void *next;         // pointer to the next block of free memory                                                  -- Free Memory or master header
} header_t;

// Helper functions **** Temporary 
void printfreemem(header_t * freemem, header_t *prevmem, header_t *masterheader);
void printeverything(header_t *masterheader, header_t *freememory, header_t *free2, header_t *free3);

// memory helper to create a plausable memory structure
void *makememory(int option);
void printlinkedlist(void *ptr, int nodesscriptions);
void *makeBlockWithOneAllocation();
void *makeBlockWithAllFreeBlocks();
header_t *masterheader;


void printlinkedlist(void *ptr, int nodescriptions)
{
    header_t *usermem = (header_t *)(ptr - sizeof(header_t));
    header_t *master = (header_t *)usermem->mem_ptr;

    header_t *temp = master;
    int ct = 0;
        
    while (1)
    {
        int isMaster = 0;
        ct++;
        if (master->block_size == temp->size)
        {
            printf("This is the master header|000\n");
            isMaster = 1;
        }else
        {
            isMaster = 0;
        }
        
        if (nodescriptions)
        {
            printf("----%ld\n",  (long int)temp - (long int)master);
            //printf("%ld\n",  (long int)temp->start);

            if (temp->next == NULL)
            {
                printf("--->%ld\n", (long int)temp->next);
            }else
            {
                printf("--->%ld\n", (long int)temp->next - (long int)master); 
            }
            //printf("%ld\n",  (long int)temp->size);
            if (isMaster)
            {
                printf("%ld\n", (long int)temp->size);
            }else
            {
                printf("%ld\n", (long int)temp->size);
            }
            printf("\n");
        }else{
            printf("The address of header for node %d is  |%ld\n", ct,  (long int)temp - (long int)master);

            if (temp->next == NULL)
            {
                //printf("The start of memory block for node %d is |%ld\n", ct,  (long int)temp->start);
                printf("The next pointer for node %d is |%ld\n", ct,  (long int)temp->next);
            }else
            {
                //printf("The start of memory block for node %d is |%ld\n", ct,  (long int)temp->start);
                printf("The next pointer for node %d is |%ld\n", ct,  (long int)temp->next - (long int)master);
            }
            
            


            //printf("The size of memory block for node %d is |%ld\n", ct,  (long int)temp->size);
            if (isMaster)
            {
                printf("The size of the entire memory block with header for node number %d is |%ld\n", ct,  (long int)temp->size);
            }else
            {
                printf("The size of the entire memory block with header for node number %d is |%ld\n", ct,  (long int)(temp->size));
            }
            
        }

        temp = temp->next;
        if (temp == NULL)
        {
            return;
        }
       
        
    }


}



void *makeBlockWithOneAllocation()
{
    // Will pull a block of 1000 bytes plus extra for the master header
    size_t block_size = 1000 + sizeof(header_t);

    // Pull the big block of memory that will hold the header and the various user memory allocations
    void *ptr = mmap(0, block_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // Set up master header
    masterheader = (header_t *)ptr;
    masterheader->block_size = block_size;
    masterheader->mem_ptr = ptr;
    masterheader->size = block_size;
    masterheader->start = ptr;

    // Pull a block of memory for the user
    header_t *usr1 = (header_t *)(ptr + sizeof(header_t));
    usr1->block_size = masterheader->block_size;
    usr1->mem_ptr = masterheader->mem_ptr;
    usr1->next = NULL;
    usr1->size = 100;
    usr1->start = (void *)usr1 + sizeof(header_t);

    // set up free memory linked list
    header_t *free = (header_t *)((void *)usr1 + usr1->size);
    free->block_size = masterheader->block_size;
    free->mem_ptr = masterheader->mem_ptr;
    free->size = free->block_size - (sizeof(header_t) + usr1->size);
    free->start = (void *)free + sizeof(header_t);
    free->next = NULL;
    masterheader->next = free;
    
    return (void *)usr1->start;

}

void *makeBlockWithAllFreeBlocks()
{
    // Will pull a block of 1000 bytes plus extra for the master header
    size_t block_size = 1000 + sizeof(header_t);

    // Pull the big block of memory that will hold the header and the various user memory allocations
    void *ptr = mmap(0, block_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // Set up master header
    masterheader = (header_t *)ptr;                                 
    masterheader->block_size = block_size;
    masterheader->mem_ptr = ptr;
    masterheader->size = block_size;
    masterheader->start = ptr;
    masterheader->next = NULL;

    header_t *free1 = (void *)masterheader + sizeof(header_t);    
    free1->block_size = masterheader->block_size;
    free1->mem_ptr = masterheader->mem_ptr;
    free1->next = NULL;
    free1->size = 60;                                             
    free1->start = (void *)free1 + sizeof(header_t);               

    masterheader->next = free1;
    
    header_t *free2 = (header_t *)(free1->start + free1->size);    
    free2->block_size = masterheader->block_size;
    free2->mem_ptr = masterheader->mem_ptr;
    free2->next = NULL;
    free1->next = free2;
    free2->size = 260;                                             
    free2->start = (void *)free2 + sizeof(header_t);

    header_t *free3 = (header_t *)(free2->start + free2->size);    
    free3->block_size = masterheader->block_size;
    free3->mem_ptr = masterheader->mem_ptr;
    free3->next = NULL;
    free2->next = free3;
    free3->size = 210;                                             
    free3->start = (void *)free3 + sizeof(header_t);

    header_t *free4 = (header_t *)(free3->start + free3->size);    
    free4->block_size = masterheader->block_size;
    free4->mem_ptr = masterheader->mem_ptr;
    free4->next = NULL;
    free3->next = free4;
    free4->size = 110;                                             
    free4->start = (void *)free4 + sizeof(header_t);

    header_t *free5 = (header_t *)(free4->start + free4->size);    
    free5->block_size = masterheader->block_size;
    free5->mem_ptr = masterheader->mem_ptr;
    free5->next = NULL;
    free4->next = free5;
    free5->size = 160;                                             
    free5->start = (void *)free5 + sizeof(header_t);

    
    return (void *)free1->start;
    
}



void *makememory(int option)
{
    // Will pull a block of 1000 bytes plus extra for the master header
    size_t block_size = 1000 + sizeof(header_t);

    // Pull the big block of memory that will hold the header and the various user memory allocations
    void *ptr = mmap(0, block_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // Create the master header
    masterheader = (header_t *)ptr;

    // Set the master headers attributes
    masterheader->mem_ptr = ptr;
    masterheader->block_size = block_size;      // For the master header set the block_size and size to the same value
    masterheader->start = ptr;                //  This is 0 for the master header
    masterheader->next = NULL;              //  Will be set to the memory adress of the first free block header
    masterheader->size = block_size;                 // the size parameter is reserved for user blocks of memory not the master header

    

    // Pointer for the fisrt and as of yet only free block
    header_t *freememory = (header_t *)(masterheader->mem_ptr + sizeof(header_t));      // Set the start after the master header
    freememory->mem_ptr = masterheader->mem_ptr;                                        // pointer that adentifies what mmap the master block belongs to
    freememory->start = (void *)freememory + sizeof(header_t);                          // Where the usable memory begins
    freememory->size = masterheader->block_size - (sizeof(header_t) * 2);               // size of the usable memory block
    freememory->next = NULL;

    masterheader->next = freememory;
    

    
    
    header_t *usrptr1 = freememory;         
                      

    
    freememory = (header_t *)(ptr + 180);
    usrptr1->size = 140;
    freememory->size = 280;
    freememory->mem_ptr = masterheader->mem_ptr;                                        
    freememory->start = (void *)freememory + sizeof(header_t);
    freememory->next = NULL;
    masterheader->next = freememory;
    usrptr1->mem_ptr = masterheader->mem_ptr;
    usrptr1->block_size = masterheader->block_size;
    
    usrptr1->start = (void *)usrptr1 + sizeof(header_t);


    header_t *usrptr2 = masterheader->mem_ptr + 460;

    usrptr2->mem_ptr = masterheader->mem_ptr;
    usrptr2->block_size = masterheader->block_size;
    usrptr2->size = 90;
    usrptr2->start = (void *)usrptr2 + sizeof(header_t);

    
    //freememory->size = (void *)usrptr2 - freememory->start;

    header_t *free2 = (void *)usrptr2 + usrptr2->size;
    freememory->next = free2;
    free2->mem_ptr = masterheader->mem_ptr;
    free2->start = (void *)free2 + sizeof(header_t);
    free2->size = 110;
    free2->next = NULL;

    


    header_t *usrptr3 = masterheader->mem_ptr + 660;
    
    usrptr3->mem_ptr = masterheader->mem_ptr;
    usrptr3->block_size = masterheader->block_size;
    usrptr3->size = 140;
    usrptr3->start = (void *)usrptr3 + sizeof(header_t);

    

    header_t *free3 = (void *)usrptr3 + usrptr3->size;
    free2->next = free3;
    free3->mem_ptr = masterheader->mem_ptr;
    free3->start = (void *)free3 + sizeof(header_t);
    free3->size = 100;
    free3->next = NULL;

    //printeverything(masterheader, freememory, free2, free3);
    

    header_t *usrptr4 = masterheader->mem_ptr + 900;
    
    usrptr4->mem_ptr = masterheader->mem_ptr;
    usrptr4->block_size = masterheader->block_size;
    usrptr4->size = 140;
    usrptr4->start = (void *)usrptr4 + sizeof(header_t);

    

    






    //return usrptr2->start;
    if (option == 1)
    {
        return usrptr1->start;
    }else if (option == 2)
    {
        return usrptr2->start;
    }else  if (option == 3)
    {
        return usrptr3->start;
    }else  if (option == 4)
    {
        return usrptr4->start;
        
    }
    
    
    
    //munmap(ptr, block_size);
}







void printfreemem(header_t * freemem, header_t *prevmem, header_t *masterheader)
{
    printf("\n\n****************************\n");
    printf("Address of freemem is \t %ld\n", (long int)((void *)freemem));
    printf("The previous headers next points to \t %ld\n", (long int)(prevmem->next));
    printf("The amount of free memory is %ld bytes\n", freemem->size);
    printf("The start of the free memory is %ld bytes\n", (long int)((void *)freemem->start));
    printf("The last address of the freemem is is %ld\n", (long int)((void *)freemem->start) + freemem->size);
    printf("****************************\n");
    printf("The Master headers start address is %ld\n", (long int)masterheader);
    printf("The masterheaders total size is %ld\n", masterheader->block_size);
    long diff = (long int)masterheader + masterheader->size;
    printf("The last address of the block should be %ld\n", diff);
}


void printeverything(header_t *masterheader, header_t *freememory, header_t *free2, header_t *free3)
{
    // Shows me how large the header is
    printf("Size of the header \t\t\t\t\t\t %ld bytes\n", sizeof(header_t));

    // Print the address of the master header
    printf("Address of the pointer to masterheaders header\t\t\t %ld\n", (long int)masterheader);
   
    // Print the sze of the block of memory the master header owns
    printf("The size of user memory is\t\t\t\t\t %ld bytes\n", masterheader->block_size - sizeof(header_t));

    printfreemem(freememory, masterheader, masterheader);
    printfreemem(free2, freememory, masterheader);
    printfreemem(free3, free2, masterheader);


    printf("%ld\n", (long int)((void *)masterheader));                  // 39
    printf("%ld\n", (long int)((void *)masterheader->next));            // 40
    printf("%ld\n", (long int)((void *)masterheader->block_size));      // 41
    printf("%ld\n", (long int)((void *)freememory));                    // 42
    printf("%ld\n", (long int)((void *)freememory->next));              // 43
    printf("%ld\n", (long int)((void *)freememory->start));             // 44
    printf("%ld\n", (long int)((void *)free2));                         // 45
    printf("%ld\n", (long int)((void *)free2->next));                   // 46
    printf("%ld\n", (long int)((void *)free2->start));                  // 47
    printf("%ld\n", (long int)((void *)free2->size));                   // 48
    printf("%ld\n", (long int)((void *)free3));                         // 49
    printf("%ld\n", (long int)((void *)free3->next));                   // 50
    printf("%ld\n", (long int)((void *)free3->start));                  // 51
    printf("%ld\n", (long int)((void *)free3->size));                   // 52
    printf("%ld\n", (long int)((void *)freememory->size));              // 53
}



