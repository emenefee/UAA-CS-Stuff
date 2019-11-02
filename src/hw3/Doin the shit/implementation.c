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

// typedef struct{
//     void *mmap_block; // the address of the whole memory block used to uniquly identify the mmap it belongs to.        -- All will be the same for all
//     size_t mmap_size; // size of the large block allocation                                                        -- Mater Header
//     size_t node_size; // Size of the block given to the user minus the Header                                           -- User Memory
//     void *node_start; // Start of the block of usable memory minus the header                                            -- User Memory
//     header_t* next_block; // pointer to the next block of free memory                                                     -- Free Memory
// } header_t;

typedef struct{
    void *mmap_block;      // the address of the whole memory block used to uniquly identify the mmap it belongs to.    -- All will be the same for all
    size_t mmap_size;  // size of the large block allocation                                                        -- All will be the same for all
    size_t node_size;        // Size of the block given to the user including the header                                  -- User Memory 
    void *node_start;        // Start of the block of usable memory minus the header                                      -- User Memory
    void *next;         // pointer to the next block of free memory                                                  -- Free Memory or master header
} header_t;

header_t* free_mem = NULL;

#define DEFAULT_MEM_SIZE ((size_t)(16777216))
#define HEADER_SIZE sizeof(header_t)

// Basically just a plain call to mmap
static void* map_mem(size_t size)
{
  void* ptr = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
  return ptr;
}

// Helps figure out if the size needs to be default or customized.
static void* size_helper(size_t size)
{
    void * ptr;
  // If size wanting to be allocated is larger than the default size
  if(size > DEFAULT_MEM_SIZE || (size + HEADER_SIZE) > DEFAULT_MEM_SIZE)
  {
    ptr = map_mem(sizemem(DEFAULT_MEM_SIZE, size));
  }
  else
  {
    ptr = map_mem(DEFAULT_MEM_SIZE);
  }
  return ptr;
}

// Adding a free block to the ll of free blocks
// ** ADDING TO THE FREE LL
// NOTE: We do not care about which mmap is where in the add function
static void add_to_ll(void* add, size_t size)
{
  // Cast given pointer to a header_t object
  header_t* block_to_add = (header_t*)add;
  // Make the start point of the node equal to the pointer passed into the function
  block_to_add->node_start = add;

  if(free_mem == NULL) // If the free_mem LL is empty
  {
    /* address of the begining of the mmap = pointer given by add
       size of the mmap = the size of the pointer(????) CHECK
       node size = mmap_size (basically)
       next block = null (because its the first one)
       initilize free_mem */
    block_to_add->mmap_block = add;
    block_to_add->mmap_size = size;
    block_to_add->node_size = size;
    block_to_add->next_block = NULL;
    free_mem = block_to_add;
  }
  else // Free list is not empty
  {
    header_t* curr_block = free_mem; // Start at the begining of free memory
    header_t* next_block free_mem->next_block; // Find the begining of the next block

    while(curr_block != NULL)
    {
      if(curr_block->node_start < add->node_start) // If the address of the current node is less than the node to be inserted
      {
        block_to_add->node_next = next_block; // Assign the 'next' var of the node to be inserted to the next node of current
        block_to_add->node_size = size;
        block_to_add->mmap_size = curr_block->mmap_size;
        block_to_add->mmap_block = curr_block->mmap_block;
        curr_block->node_next = block_to_add; // Assign currents next to the node to be inserted
        break;
      }
      // Increment current pointer
      curr_block = next_block;
      next_block = curr_block->next;
    }
    coallace();
  }
}

// When malloc is called, you 'remove' free memory from the header
// ** ALLOCATING MEMORY
// Note: Dont care what mmap the remove comes from.
static void rmv_from_ll(header_t* rmv, size_t size)
{
  size_t size = rmv->node_size;

  header_t* curr_mem = free_mem; // Start at the begining
  header_t* prev = NULL;

  while(curr_mem != NULL)
  {
    if(curr_mem == rmv)
      break;

    prev = curr_mem; // Prev = current
    curr_mem = curr_mem->node_next;
  }

  if(curr_mem == rmv) // If you're allocating memory from the first free_mem pointer
  {
    header_t* new_ptr = (header_t*)(rmv->mmap_block + size);
    new_ptr->mmap_block = rmv->mmap_block;
    new_ptr->mmap_size = rmv->mmap_size;
    new_ptr->node_size = (rmv->mmap_size - size);
    new_ptr->node_start = (rmv->mmap_block + size);
    new_ptr->node_next = rmv->node_next;
    free_mem = new_ptr;
  }
  else 
  {
    prev->next = curr_mem->next;
  }
  curr_mem->next = NULL;
  rmv->next = NULL;
  coallace();
}

// Smooshing together blocks that need to be smushed
static void coallace()
{
  header_t* curr = free_mem;
  header_t* next = curr->next;

  size_t curr_end, next_start;
  void* curr_block;
  void* next_block;

  while(curr != NULL)
  {
    // End of current block -- CONFUSING AF
    curr_end = curr_block + curr_block->size;
    // Start of previous block
    // next_start = (size_t)(next->node_size - HEADER_SIZE);

    // mmap curr & next belong to
    curr_block = curr->mmap_block;
    next_block = next->mmap_block;

    // If the blocks are right next to eachother & they are in the same mmap
    if((curr_end == next_block) && (curr_block_size == next_block_size))
    {
      curr->node_size = curr->node_size + (size_t)(next->node_size); //Size of current node = current size + next size + 
                                                                                    // size of header (from next)
      curr->next = next->next;
    }
    else // increment curr & next
    {
      curr = next;
      next = curr->next;
    }
  }
}


/* End of your helper functions */

/* Start of the actual malloc/calloc/realloc/free functions */

void __free_impl(void *);

void *__malloc_impl(size_t size) {
  int found_mem = 0; // didn't like the bool

  header_t* curr_node = free_mem;
  void* ptr;
  size_t curr_node_size;

  // Blanket Condition: if the size given is 0, return null
  if(size == 0) return NULL;

  // If the LL is empty
  else if(curr_node == NULL)
  {
    // ptr to new mmap
    ptr = size_helper(size);
    add_to_ll(ptr, size);
    __malloc_impl(size);
  }
  else // if LL is not empty
  {
    while(curr_node != NULL)
    {
      curr_node_size = curr_node->node_size; //assign curr_mem_size to the size of the current free block
      if(size <= (curr_node_size - HEADER_SIZE)) // if the size of mem to allocate can fit in a free block
      {
        found_mem = 1;
        rmv_from_ll(curr_node);
        return (curr_node->node_start + HEADER_SIZE); // return the start of said free block
      }
      curr_node = curr_node->node_next;
    }

    //if you did not a free block
    if(!found_mem)
    {
      ptr = size_helper(size);
      add_to_ll(ptr, size);
      __malloc_impl(size)
    }
  }
}

void *__calloc_impl(size_t nmemb, size_t size) {
  size_t* what_dis;
  int success = __try_size_t_multiply(what_dis, nmeb, size);
  if(success)
  {
    void* ptr = __malloc_impl((nmemb*size));
    void* return_ptr = __memset(ptr, 0, what_dis);
    return return_ptr;
  }
  else return NULL;  
}

void *__realloc_impl(void *ptr, size_t size) {
  size_t ptr_size = ptr->size
  if(ptr == NULL) 
  {
    void* new_ptr = __malloc_impl(size);
  }
  else if (size == 0)
  {
    free(ptr);
  }
  else if (size < ptr_size)
  {
    // NOTE: size still includes headers and whatnot
    header_t* to_add = NULL;
    to_add->mem_ptr = ptr->mem_ptr;
    to_add->block_size = ptr->block_size;
    size_t new_size = ptr_size - size;
    to_add->size = new_size;
    // TODO: figure out how to add a next pointer
    // Add it to the LL (need a function??? I have a function)
    __free_impl(to_add);
  }
  else if (size > ptr_size)
  {
    void* new_ptr = __malloc_impl(size);
    new_ptr = __memcpy(new_ptr, ptr, (size - HEADER_SIZE));

    /* TODO: free old block, return new block */
  }
  return NULL;  
}

void __free_impl(void *ptr) {
  /* STUB */
}

/* End of the actual malloc/calloc/realloc/free functions */

