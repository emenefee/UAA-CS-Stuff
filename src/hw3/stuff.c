//     Matt Hakin & Elysha Menefee    //
//          Date: Nov 3, 2019         //
// HW3: malloc, calloc, realloc, free //

#define DEFAULT_MEM_SIZE ((size_t) (16777216))				

// NOTE:::::::: I have kind of added/changed stuff to the struct
// https://www.learn-c.org/en/Linked_lists

typedef struct{
    void *head_ptr; // the address of the whole memory block used to uniquly identify the mmap it belongs to.        -- All will be the same for all
    size_t head_size; // size of the large block allocation                                                        -- Mater Header
    size_t node_size; // Size of the block given to the user minus the Header 	                                        -- User Memory
    void *node_start; // Start of the block of usable memory minus the header                                            -- User Memory
    header_t* next; // pointer to the next block of free memory                                                     -- Free Memory
} header_t;

header_t* free_mem = NULL;

void* map_mem(size_t size)
{
	void* ptr = mmap(0, DEFAULT_MEM_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	return ptr;
}

void* malloc(size_t size)
{
	bool found_mem = FALSE;
	node * curr_free_mem;
	size_t curr_mem_size;

	size_t header_size = sizeof(header_t); // grab the size of a header struct?

	// Blanket Condition: if the size given is 0 or null, return null
	if(size == 0 || size == NULL)
		return NULL;

	// Check to see if the list of free vars is populated
	if(curr_free_mem == NULL) //if no memory is available
	{
		void* ptr = map_mem(DEFAULT_MEM_SIZE);
		return ptr;
	}

	// Loop through list of free blocks to find a place to 'allocate' usermem to
	while(curr_free_mem->next != NULL)
	{
		curr_mem_size = curr_free_mem->size; //assign curr_mem_size to the size of the current free block
		if(curr_mem_size >= size) // if the size of mem to allocate can fit in a free block
		{
			found_mem = TRUE;
			return curr_free_mem->start; // return the start of said free block
		}
		curr_free_mem = curr_free_mem->next;
	}

	// If a segment has not been found & curr_mem_size == NULL
	if((!found_mem) && (curr_mem_size == NULL))
	{
		if((size + header_size) <= DEFAULT_MEM_SIZE)
		{
			void* ptr = map_mem(DEFAULT_MEM_SIZE);
			return ptr;
		}
		else if ((size + header_size) > DEFAULT_MEM_SIZE)
		{
			size_t size_to_allocate;
			// Some magic shit happens here man idek
		}
	}
}


void free(void* ptr)
{
	void* temp_ptr = ptr - sizeof(header_t);
	temp_ptr->node_size = size_of_temp;

}

void add_to_ll(header_t insert)
{
	if(free_mem == NULL)
	{
		free_mem = insert;
	}
	else
	{
		header_t* curr_head = free_mem;
		while(curr_head != NULL)
		{
			if(curr_head->node_start > insert->node_start)
			{
				
			}
			else if(curr_head->node_start < insert->node_start)
			{

			}
		}
	}
}

void rmv_from_ll(header_t rmv)
{

}

void coallace()
{
	
}