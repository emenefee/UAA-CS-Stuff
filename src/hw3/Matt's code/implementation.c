/*  

    Copyright 2018-19 by

    University of Alaska Anchorage, College of Engineering.

    All rights reserved.

    Contributors:  ...
		   ...                 and
		   Christoph Lauter

    See file memory.c on how to compile this code.

    Implement the functions __malloc_impl, __calloc_impl,
    __realloc_impl and __free_impl below. The functions must behave
    like malloc, calloc, realloc and free but your implementation must
    of course not be based on malloc, calloc, realloc and free.

    Use the mmap and munmap system calls to create private anonymous
    memory mappings and hence to get basic access to memory, as the
    kernel provides it. Implement your memory management functions
    based on that "raw" access to user space memory.

    As the mmap and munmap system calls are slow, you have to find a
    way to reduce the number of system calls, by "grouping" them into
    larger blocks of memory accesses. As a matter of course, this needs
    to be done sensibly, i.e. without wasting too much memory.

    You must not use any functions provided by the system besides mmap
    and munmap. If you need memset and memcpy, use the naive
    implementations below. If they are too slow for your purpose,
    rewrite them in order to improve them!

    Catch all errors that may occur for mmap and munmap. In these cases
    make malloc/calloc/realloc/free just fail. Do not print out any 
    debug messages as this might get you into an infinite recursion!

    Your __calloc_impl will probably just call your __malloc_impl, check
    if that allocation worked and then set the fresh allocated memory
    to all zeros. Be aware that calloc comes with two size_t arguments
    and that malloc has only one. The classical multiplication of the two
    size_t arguments of calloc is wrong! Read this to convince yourself:

    https://cert.uni-stuttgart.de/ticker/advisories/calloc.en.html

    In order to allow you to properly refuse to perform the calloc instead
    of allocating too little memory, the __try_size_t_multiply function is
    provided below for your convenience.
    
*/

#include <stddef.h>
#include <sys/mman.h>

/* Predefined helper functions */

static void *__memset(void *s, int c, size_t n) {
  unsigned char *p;
  size_t i;

  if (n == ((size_t) 0)) return s;
  for (i=(size_t) 0,p=(unsigned char *)s;
       i<=(n-((size_t) 1));
       i++,p++) {
    *p = (unsigned char) c;
  }
  return s;
}

static void *__memcpy(void *dest, const void *src, size_t n) {
  unsigned char *pd;
  const unsigned char *ps;
  size_t i;

  if (n == ((size_t) 0)) return dest;
  for (i=(size_t) 0,pd=(unsigned char *)dest,ps=(const unsigned char *)src;
       i<=(n-((size_t) 1));
       i++,pd++,ps++) {
    *pd = *ps;
  }
  return dest;
}

/* Tries to multiply the two size_t arguments a and b.

   If the product holds on a size_t variable, sets the 
   variable pointed to by c to that product and returns a 
   non-zero value.
   
   Otherwise, does not touch the variable pointed to by c and 
   returns zero.

   This implementation is kind of naive as it uses a division.
   If performance is an issue, try to speed it up by avoiding 
   the division while making sure that it still does the right 
   thing (which is hard to prove).

*/
static int __try_size_t_multiply(size_t *c, size_t a, size_t b) {
  size_t t, r, q;

  /* If any of the arguments a and b is zero, everthing works just fine. */
  if ((a == ((size_t) 0)) ||
      (a == ((size_t) 0))) {
    *c = a * b;
    return 1;
  }

  /* Here, neither a nor b is zero. 

     We perform the multiplication, which may overflow, i.e. present
     some modulo-behavior.

  */
  t = a * b;

  /* Perform Euclidian division on t by a:

     t = a * q + r

     As we are sure that a is non-zero, we are sure
     that we will not divide by zero.

  */
  q = t / a;
  r = t % a;

  /* If the rest r is non-zero, the multiplication overflowed. */
  if (r != ((size_t) 0)) return 0;

  /* Here the rest r is zero, so we are sure that t = a * q.

     If q is different from b, the multiplication overflowed.
     Otherwise we are sure that t = a * b.

  */
  if (q != b) return 0;
  *c = t;
  return 1;
}

/* End of predefined helper functions */

/* Your helper functions 

   You may also put some struct definitions, typedefs and global
   variables here. Typically, the instructor's solution starts with
   defining a certain struct, a typedef and a global variable holding
   the start of a linked list of currently free memory blocks. That 
   list probably needs to be kept ordered by ascending addresses.

*/


//     Matt Hakin & Elysha Menefee    //
//          Date: Nov 3, 2019         //
// HW3: malloc, calloc, realloc, free //

#define DEFAULT_MEM_SIZE ((size_t) (16777216))


typedef struct{
    void *mem_ptr;      // the address of the whole memory block used to uniquly identify the mmap it belongs to.    -- All will be the same for all
    size_t block_size;  // size of the large block allocation                                                        -- All will be the same for all
    size_t size;        // Size of the block given to the user including the header                                  -- User Memory 
    void *start;        // Start of the block of usable memory minus the header                                      -- User Memory
    void *next;         // pointer to the next block of free memory                                                  -- Free Memory or master header
} header_t;

// Pointer to hold the blocks of free memory
header_t *freememory = NULL;

// Helper function used to total 
// up the memory to determine if 
// all blocks have been freed
size_t CountMemory(void *memory);

// helper function to pull memory from the heap
void* map_mem(size_t size);

// if the user is asking for more memory than the default
// this function will resize what is allocated.
size_t sizemem(size_t original, size_t requested);

// function to find a free block big enough to handle
// a users request for memory
header_t *findFreeBlock(size_t needed_size);

// find the previous free block
header_t *prev(header_t *ptr);

// Utility to find the last free block
header_t *findLastFreeblock();

/* End of your helper functions */

/* Start of the actual malloc/calloc/realloc/free functions */

void __free_impl(void *);

void *__malloc_impl(size_t size) {

  

  // Blanket Condition: if the size given is 0 or null, return null
	if(size == 0)
		return NULL;


  // If no memory has been allocated
  if (freememory == NULL)
  {
    size_t NEW_DEFAULT_MEM_SIZE = DEFAULT_MEM_SIZE;

    // see if the default memory size can handle the user request
    // make sure to acount for the space taken by the freememory header 
    // and the header of the new usr memory block
    if (size > (DEFAULT_MEM_SIZE - (sizeof(header_t) * 2)))
    {
        NEW_DEFAULT_MEM_SIZE = sizemem(DEFAULT_MEM_SIZE, size + (sizeof(header_t) * 2));
    }


    freememory = (header_t *)map_mem(NEW_DEFAULT_MEM_SIZE);

    // block_size refers to the total amount of memory 
    // pulled by mmap
    freememory->block_size = NEW_DEFAULT_MEM_SIZE;
    freememory->mem_ptr = (void *)freememory;

    freememory->size = NEW_DEFAULT_MEM_SIZE;
    
    // Allocate new pointer for the user block of memory
    void *ptr = (void *)freememory + sizeof(header_t);

    // Create the header for the user block of memory
    header_t *usrmem = (header_t *)ptr;

    // Set the users block of memory header
    // these are used to help other functions find the mmap
    // the block belongs to
    usrmem->block_size = freememory->block_size;
    usrmem->size = freememory->block_size;

   
    // Set the start equal to the starting address of the 
    // useable part of the memory block
    usrmem->start = (void *)usrmem + sizeof(header_t);

    // Set the size of the users block to the size required
    // plus the size of its header
    usrmem->size = size + sizeof(header_t);
    
    // Active user blocks need to have their next pointer set to NULL
    usrmem->next = NULL;

    // Set up first block of free memory
    //                  (header_t *)((start address of freememory) + (size of freememory's header) + (size of the user memory block))
    header_t *newfree = (header_t *)(ptr + sizeof(header_t) + usrmem->size);
    newfree->block_size = freememory->block_size;
    newfree->mem_ptr = (void *)freememory;

    // The new free memory blocks size is everythig left after the size of the freememory header
    // and the usermemy is subtracted
    newfree->size = freememory->block_size - (sizeof(header_t) + usrmem->size);
    newfree->next = NULL;

    // Set the global variable "freememory's" next 
    // to this newly created free block of memory
    freememory->next = newfree;


    // Return the start of the new user memory
    return usrmem->start;
  }else
  {
    header_t *freeblock = findFreeBlock(size);

    if (freeblock != NULL)
    {
      // do the things to use it

      // find the header of the free block preceeding this block
      header_t *prevHeader = prev(freeblock);

      // Set previous->next to the next block
      prevHeader->next = freeblock->next;

      // Blocks of memory that are in use have
      // their next ptr set to null
      freeblock->next = NULL;

      // return the pointer of the block's useable memory
      return freeblock->start;
    }else
    {
      // allocate a new block of sufficient size
      size_t NEW_DEFAULT_MEM_SIZE = DEFAULT_MEM_SIZE;


      if (size > (DEFAULT_MEM_SIZE - (sizeof(header_t) * 2)))
      {
          NEW_DEFAULT_MEM_SIZE = sizemem(DEFAULT_MEM_SIZE, size + (sizeof(header_t) * 2));
      }

      
      header_t *newalloc = (header_t *)map_mem(NEW_DEFAULT_MEM_SIZE);

      // block_size refers to the total amount of memory 
      // pulled by mmap
      newalloc->block_size = NEW_DEFAULT_MEM_SIZE;
      newalloc->mem_ptr = (void *)newalloc;

      newalloc->size = NEW_DEFAULT_MEM_SIZE;
      
      // Allocate new pointer for the user block of memory
      void *ptr = (void *)newalloc + sizeof(header_t);

      // Create the header for the user block of memory
      header_t *usrmem = (header_t *)ptr;

      // Set the users block of memory header
      // these are used to help other functions find the mmap
      // the block belongs to
      usrmem->block_size = newalloc->block_size;
      usrmem->size = newalloc->block_size;

    
      // Set the start equal to the starting address of the 
      // useable part of the memory block
      usrmem->start = (void *)usrmem + sizeof(header_t);

      // Set the size of the users block to the size required
      // plus the size of its header
      usrmem->size = size + sizeof(header_t);
      
      // Active user blocks need to have their next pointer set to NULL
      usrmem->next = NULL;

      // Set up first block of free memory
      //                  (header_t *)((start address of newalloc) + (size of newalloc's header) + (size of the user memory block))
      header_t *newfree = (header_t *)(ptr + sizeof(header_t) + usrmem->size);
      newfree->block_size = newalloc->block_size;
      newfree->mem_ptr = (void *)newalloc;

      // The new free memory blocks size is everythig left after the size of the newalloc header
      // and the usermemy is subtracted
      newfree->size = newalloc->block_size - (sizeof(header_t) + usrmem->size);
      newfree->next = NULL;

      // Set the global variable "newalloc's" next 
      // to this newly created free block of memory
      newalloc->next = newfree;

      findLastFreeblock()->next = newalloc;
      // Return the start of the new user memory
      return usrmem->start;

      
    }
    
    /* there is already a block allocated
      TO DO:
        - See if there is a free block that can accomodate the user request
        - If there is a free block that can fit it, create new memory allocation
          from that block
        - if there isn't, the memory will need to be expanded and new pointer created
        - sort all the pointers and return the new block of memory
    */
    return NULL;
  }
  
  


  
}

void *__calloc_impl(size_t nmemb, size_t size) {
  /* STUB */
  return NULL;  
}

void *__realloc_impl(void *ptr, size_t size) {
  /* STUB */
  return NULL;  
}

void __free_impl(void *ptr) {
     
    // get the header to the memory block being passed in.
    header_t *usrmem = (header_t *)(ptr - sizeof(header_t));

    // Set the header of the local mmap
    header_t *tempMasterHeader = (header_t *)usrmem->mem_ptr;
    // These two pointers are used to loop through the free memory 
    // Linked list. Once a free memory header has been found with 
    // an address greater than the usrmem address, free knows that the 
    // prev pointer belongs to the free block header that sits before the 
    // block being freed
    header_t *prev = tempMasterHeader;
    header_t *temp = tempMasterHeader;

    // This is a temporary pointer used to hold the address
    // of the free memory block just before the block being freed
    // ***** Not the same as freememory
    header_t *free = NULL;

    // Loop through the Linked list to find the approprate free memory header
    // that comes just before the block being freed
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
    if (free == tempMasterHeader)
    {
        usrmem->next = tempMasterHeader->next;
        tempMasterHeader->next = usrmem;
    }else
    {
        usrmem->next = free->next;
        free->next = usrmem;
    }

    // If all the memory has been released, call mumap
    if (CountMemory((void *)tempMasterHeader + sizeof(header_t)) >= (tempMasterHeader->block_size - sizeof(header_t)))
    {
      if (tempMasterHeader == freememory)
      {
         if (tempMasterHeader->next == NULL)
         {
           munmap(tempMasterHeader->mem_ptr, tempMasterHeader->block_size);
         }else
         {
           freememory = tempMasterHeader->next;
           munmap(tempMasterHeader->mem_ptr, tempMasterHeader->block_size);
         }
         
      }else
      {
        // set the previois master header to point at is next free block
        free->next = tempMasterHeader->next;

        munmap(tempMasterHeader->mem_ptr, tempMasterHeader->block_size);
        tempMasterHeader = NULL;
      }
      
        

        // Set the global pointer of freememory to null
        tempMasterHeader = NULL;
    }

    return;
}

/* End of the actual malloc/calloc/realloc/free functions */










// helper function to pull memory from the heap
void* map_mem(size_t size)
{
	void* ptr = mmap(0, DEFAULT_MEM_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	return ptr;
}

// Used to total up the memory to determine if 
// all blocks have been freed
size_t CountMemory(void *memory)
{
    // Grab the header of the user block of memory
    header_t *temp = (header_t *)(memory - sizeof(header_t));
    header_t *tempHead = temp->mem_ptr; // This is the master head of the local allocation
    
    // This is used for the while loop to determine 
    // when the freeheadr is found
    int end = 0;

    // To store the sum of the sizes of all the free memory blocks
    // the "sizeof(header_t)" is to account for the space of the
    // freememory header
    long memct = sizeof(header_t);


    //header_t *mstr = (header_t *)temp->mem_ptr;
    temp = tempHead->next;
    while (!end)
    {
        memct = memct + temp->size;
        // only count memory in this block
        if (temp->next == NULL || temp->mem_ptr != tempHead->mem_ptr)
        {
            end = 1;
        }
        temp = temp->next;
    }

    return (size_t)memct;
}

// if the user is asking for more memory than the default
// this function will resize what is allocated.
size_t sizemem(size_t original, size_t requested)
{
    float needed = (float)requested;
    float ddefault = (float)original;
    float answer = needed / ddefault;

    long intAnswer = (long)answer;

    if (answer > intAnswer)
    {
        intAnswer++;
        return original * intAnswer;
    }else
    {
        return intAnswer;
    }
}

// this is a basic search. It finds the first suitable fit
header_t *findFreeBlock(size_t needed_size)
{
  header_t *temp = freememory;

  if (temp->next == NULL)
  {
    return NULL;
  }else
  {
    temp = temp->next;
  }
  
  while (temp != NULL)
  {
    if ((temp->size - sizeof(header_t)) > needed_size) 
    {
      return temp;
    }
    temp = temp->next;
  }
  return NULL;
}


// find the previous free block
header_t *prev(header_t *ptr)
{
    //header_t *usrmem = ptr;
    header_t *blockmem = freememory;

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

// Utility to find the last free block
// used when it is needed to 
header_t *findLastFreeblock()
{
  header_t *temp = freememory->next;
  header_t *previous_header = temp;
  if (temp != NULL)
  {
    while (temp != NULL)
    {
      previous_header = temp;
      temp = temp->next;
      if (temp == NULL)
      {
        return previous_header;
      }
    }
  }else
  {
    return freememory;
  }
  return freememory;
}