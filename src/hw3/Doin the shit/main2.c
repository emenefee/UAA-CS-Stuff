#include <stddef.h>
#include <sys/mman.h>
#define HEADER_SIZE sizeof(header_t)
#define DEFAULT_MEM_SIZE ((size_t) (16777216))

typedef struct {
    void *mmap_block;
    size_t mmap_size;
    size_t node_size;
    void *node_start;
    void* node_next;
} header_t;

// Pointer to hold the blocks of free memory
header_t *free_mem = NULL;
static void* map_mem(size_t size);
static void add_to_ll(void* add, size_t size);
static void rmv_from_ll(header_t* rmv, size_t size);
void *__malloc_impl(size_t size);
void __free_impl(void *);
static void coallace();
static void* size_helper(size_t size);
static size_t sizemem(size_t original, size_t requested);
int main(int argc, char* argv[])
{
    void * ptr = __malloc_impl(1000);
    void * ptr2 = __malloc_impl(2000);
    void * ptr3 = __malloc_impl(4000);
    void * ptr4 = __malloc_impl(17777216);
    void * ptr5 = __malloc_impl(17777216);
    void * ptr6 = __malloc_impl(17777216);
    void * ptr7 = __malloc_impl(17777216);
    void * ptr8 = __malloc_impl(17777216);
    void * ptr9 = __malloc_impl(17777216);

    header_t *looper = free_mem;
      
    while (looper != NULL)
    {
      looper = looper->node_next;
    }

    //__free_impl(ptr);
    //__free_impl(ptr2);
    //__free_impl(ptr3);
    // __free_impl(ptr4);
    
    looper = free_mem;

    while (looper != NULL)
    {
      looper = looper->node_next;

    }
    return 0;
}



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



void *__malloc_impl(size_t size) {
  int found_mem = 0; // didn't like the bool

  header_t* curr_node = free_mem;
  void* ptr;
  size_t curr_node_size, newsize;

  // Blanket Condition: if the size given is 0, return null
  if(size == 0) return NULL;

  // If the LL is empty
  else if(curr_node == NULL)
  {
    
    // Had to add this because we need to pass the size of the new node to add_to_ll
    // which in this particular case is the entire new block not the user block
    newsize = sizemem(DEFAULT_MEM_SIZE, size); 

    // ptr to new mmap
    ptr = size_helper(size);
    add_to_ll(ptr, newsize);
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

        rmv_from_ll(curr_node, size);
        return (curr_node->node_start + HEADER_SIZE); // return the start of said free block
      }
      curr_node = curr_node->node_next; 
    }

    //if you did not a free block
    if(!found_mem) // BP
    {
      newsize = sizemem(DEFAULT_MEM_SIZE, size); 

      ptr = size_helper(size);
      add_to_ll(ptr, newsize);
      __malloc_impl(size);
    }
  }
}

void *__calloc_impl(size_t nmemb, size_t size) {
  /* size_t* what_dis;
  int success = __try_size_t_multiply(what_dis, nmeb, size);
  if(success)
  {
    void* ptr = __malloc_impl((nmemb*size));
    void* return_ptr = __memset(ptr, 0, what_dis);
    return return_ptr;
  }
  else */ return NULL;  
}

void *__realloc_impl(void *ptr, size_t size) {
  /* size_t ptr_size = ptr->size
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
    new_ptr = __memcpy(new_ptr, ptr, (size - HEADER_SIZE)); */

    /* TODO: free old block, return new block */
  //}
  return NULL;   
}

void __free_impl(void *ptr) {
     

    return;
}

/* End of the actual malloc/calloc/realloc/free functions */



// helper function to pull memory from the heap
static void* map_mem(size_t size)
{
	void* ptr = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	return ptr;
}

// When malloc is called, you 'remove' free memory from the header
// ** ALLOCATING MEMORY
// Note: Dont care what mmap the remove comes from.
static void rmv_from_ll(header_t* rmv, size_t size)
{
  //size_t size = rmv->node_size;

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
    header_t* new_ptr = (header_t*)((void*)rmv + size + HEADER_SIZE); // Giving new_ptr the address
    new_ptr->mmap_block = rmv->mmap_block;
    new_ptr->mmap_size = rmv->mmap_size;
    new_ptr->node_size = (rmv->mmap_size - size);
    // (HEADER_SIZE*2) is because the intial size does not account for a header on the userblock
    new_ptr->node_start = ((void*)rmv + size + HEADER_SIZE);
    new_ptr->node_next = rmv->node_next;
    if(curr_mem == free_mem && prev == NULL)
    {
      free_mem = new_ptr;//->node_start;
    }
    else
    {
      prev->node_next = curr_mem;
    }

  }
  else 
  {
    prev->node_next = curr_mem->node_next;
  }
  curr_mem->node_next = NULL;
  rmv->node_next = NULL;
  //coallace();
}

// Adding a free block to the ll of free blocks
// ** ADDING TO THE FREE LL
// NOTE: We do not care about which mmap is where in the add function
static void add_to_ll(void* add, size_t size)
{
  header_t* block_to_add = (header_t*)add;

  // Make the start point of the node equal to the pointer passed into the function
  block_to_add->node_start = (void*)add;

  if(free_mem == NULL) // If the free_mem LL is empty
  {
    header_t* first_header = (header_t*)add;
    first_header->mmap_block = add;
    first_header->mmap_size = size;
    first_header->node_size = size;
    first_header->node_start = (void*)(add);// + HEADER_SIZE);
    free_mem = first_header;
  }
  else
  {
    header_t* curr_block = free_mem; // Start at the begining of free memory
    header_t* next_block = free_mem->node_next; // Find the begining of the next block

    while(curr_block != NULL)
    {
      if(curr_block->node_start < block_to_add->node_start) // If the address of the current node is less than the node to be inserted
      {
        block_to_add->node_next = next_block; // Assign the 'next' var of the node to be inserted to the next node of current
        block_to_add->node_size = size;
        block_to_add->mmap_size = curr_block->mmap_size;
        block_to_add->mmap_block = curr_block->mmap_block;
        curr_block->node_next = block_to_add; // Assign currents next to the node to be inserted
        break;
      }
      // If there is only one node in the LL & the address of the new block is before the old block
      // Adding a whole new mmap to the front of the free_mem list
      else if (curr_block->node_next == NULL && curr_block->node_start > block_to_add->node_start)
      {
        block_to_add->mmap_size = size;
        block_to_add->mmap_block = add;
        block_to_add->node_size = size;
        block_to_add->node_start = add;
        block_to_add->node_next = curr_block;
        free_mem = block_to_add;
        break;
      }
      // Increment current pointer
      curr_block = next_block;
      next_block = curr_block->node_next;
    }
    //coallace();
  }
}

// if the user is asking for more memory than the default
// this function will resize what is allocated.
static size_t sizemem(size_t original, size_t requested)
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

void loopthrough()
{
    header_t *temp;
    temp = free_mem->node_next;
    while(temp != NULL)
    {
        //size_t mysize = temp->size;
        long myAddress = (long int)((void *)temp);
        long mystart = (long int)temp->node_start;

        //long total = sizeof(header_t) + mystart + mysize;
        temp = temp->node_next;
    }
}

// Smooshing together blocks that need to be smushed
static void coallace()
{
  /* header_t* curr = free_mem;
  header_t* next = curr->next;

  size_t curr_end, next_start;
  void* curr_block;
  void* next_block;

  while(curr != NULL)
  {
    // End of current block -- CONFUSING AF
    curr_end = curr_block + curr_block->size;
    // Start of previous block
    //next_start = (size_t)(next->node_size - HEADER_SIZE);

    // mmap curr & next belong to
    curr_block = curr->mmap_block;
    next_block = next->mmap_block;

    // If the blocks are right next to eachother & they are in the same mmap
    if((curr_end == next_block) && (curr_block_size == next_block_size))
    {
      curr->node_size = curr->node_size + (size_t)(next->nodesize); //Size of current node = current size + next size + 
                                                                                    // size of header (from next)
      curr->next = next->next;
    }
    else // increment curr & next
    {
      curr = next;
      next = curr->next;
    }
  } */
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