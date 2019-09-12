#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "mem.h"

////////////////////////////////////////////////////////////////////////////////
// Main File:        mem.c
// This File:        mem.c
// Other Files:      None
// Semester:         CS 354 Spring 2019
//
// Author:           Theodore Montalbano
// Email:            tmontalbano@wisc.edu
// CS Login:         montalbano
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of 
//                   of any information you find.
//////////////////////////// 80 columns wide ///////////////////////////////////

/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footer for each free block but only containing size.
 */
typedef struct block_header {
        int size_status;
    /*
    * Size of the block is always a multiple of 8.
    * Size is stored in all block headers and free block footers.
    *
    * Status is stored only in headers using the two least significant bits.
    *   Bit0 => least significant bit, last bit
    *   Bit0 == 0 => free block
    *   Bit0 == 1 => allocated block
    *
    *   Bit1 => second last bit 
    *   Bit1 == 0 => previous block is free
    *   Bit1 == 1 => previous block is allocated
    * 
    * End Mark: 
    *  The end of the available memory is indicated using a size_status of 1.
    * 
    * Examples:
    * 
    * 1. Allocated block of size 24 bytes:
    *    Header:
    *      If the previous block is allocated, size_status should be 27
    *      If the previous block is free, size_status should be 25
    * 
    * 2. Free block of size 24 bytes:
    *    Header:
    *      If the previous block is allocated, size_status should be 26
    *      If the previous block is free, size_status should be 24
    *    Footer:
    *      size_status should be 24
    */
} block_header;         

/* Global variable - DO NOT CHANGE. It should always point to the first block,
 * i.e., the block at the lowest address.
 */

block_header *start_block = NULL;

/* 
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block on success.
 * Returns NULL on failure.
 * This function should:
 * - Check size - Return NULL if not positive or if larger than heap space.
 * - Determine block size rounding up to a multiple of 8 and possibly adding padding as a result.
 * - Use BEST-FIT PLACEMENT POLICY to find the block closest to the required block size
 * - Use SPLITTING to divide the chosen free block into two if it is too large.
 * - Update header(s) and footer as needed.
 * Tips: Be careful with pointer arithmetic.
 */
void* Alloc_Mem(int size) {
    // Your code goes in here.
	//Checking size to see if it is valid.
	if(size <= 0){
   		 return NULL;
	}
	//To account for the header
	size = size + sizeof(block_header);
	//Adding padding if needed
	if(size % 8 != 0){
		size = size + (8 - size%8);
	}
	//Storing "best fit" block by address and value
	int bestVal = 2147483647;
	//Above is max int value in C, value is chosen, so it is guaranteed 
	//there will be a value less than, otherwise no best block exists.
	block_header *best = NULL;

	//Block being iterated over
	block_header *current = start_block;
	
	//While there is still possible places to store left in the heap space
	while(current->size_status != 1){
		//Checks if this is the best fit, if it fits,
		//and finally if it is allocated
		if((current->size_status / 8) * 8 < bestVal &&
			       	(current->size_status / 8) * 8 >= size
			       	&& current->size_status % 2 != 1 ){
			bestVal = (current -> size_status / 8) * 8;
			//Gets rid of allocated bits when considering size.
			best = current;
		}
	//"Incrementing" Current to find next block to iterate over
	current =(block_header*)((current -> size_status / 8) * 8 + ((void*)current));
	}
	// As if the best block is still NULL then no best block
	//exists as each block has been checked
	if(best == NULL ){
		return NULL;
	}
		
	//Now check if there is enough space left to make a new block
	if(bestVal - size >= 8) {
		//Find old header and footer
		block_header *header = (block_header*)((void*)best + size);
		block_header *footer =
		       	(block_header*)((void*)best + bestVal - sizeof(block_header));

		//Update header and footer
		header -> size_status = (bestVal - size) + 2;
		footer -> size_status = (bestVal - size);

	}else{
		//If no splitting occurs next block must be updated
		block_header *toBeUpdated = ((void*)best + size);
			if(toBeUpdated -> size_status != 1){
				toBeUpdated -> size_status =
				       	toBeUpdated -> size_status + 2;
		}	
	}
	//Update header to reflect status of previous block
	if(((best -> size_status) % 8) == 2) {
		best -> size_status = size  + 3;
	} else {
		best -> size_status = size  + 1;
	}
	//Returns pointer to allocated MEM
	return ((void*)best + sizeof(block_header));
}

/* 
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - USE IMMEDIATE COALESCING if one or both of the adjacent neighbors are free.
 * - Update header(s) and footer as needed.
 */                    
int Free_Mem(void *ptr) {         
    if(ptr == NULL ||((int) ptr) % 8 != 0){
    	return -1;
    }
    // Finds size of block header, and allocated a blockheader pointer
    // to the block of memory to be manipulated
    int size = sizeof(block_header);
    block_header* free = (block_header*) (ptr - size);

    //Check if the block the pointer is pointing to is free
    if(free -> size_status % 2 == 0) {
	    return -1;
    }
    //If next block isn't allocated coalesce the two
    if(((free + ((free -> size_status) / 8) * 8)) -> size_status % 2 == 0){
	//Update the header
	free->size_status = free->size_status +
	((free + ((free->size_status) / 8) * 8)->size_status / 8) * 8;
	//The above /8 * 8 takes advantage of integer arithmetic
	//to subtract the number modulus 8 from itself
	//Update the footer
	(free + (free->size_status / 8) * 8 - size)->size_status =
	       	(free->size_status / 8) * 8;	
    }else{
	    //If it is allocated update the p bit
	    (free + (free->size_status / 8) * 8)->size_status = 
		    (free + (free->size_status / 8) * 8)->size_status - 2;
    }
    //If previous block isn't allocated
    if(free->size_status % 8 == 1 || free->size_status % 8 == 0){
	    //Coalesce update previous block's header
	   (free - (free - size)->size_status)->size_status =
		  (free - (free - size)->size_status)->size_status +
		  (free->size_status / 8) * 8;
	   //Update Pointer to new free blocks address
	   free = free - (free - size)->size_status;
	   //Update new Footer
	   (free + (free->size_status / 8) * 8 - size)->size_status =
		   free->size_status / 8 * 8;
    }else {
	    free->size_status = free->size_status - 1;
    }
    return 0;
}

/*
 * Function used to initialize the memory allocator.
 * Intended to be called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */                    
int Init_Mem(int sizeOfRegion) {         
    int pagesize;
    int padsize;
    int fd;
    int alloc_size;
    void* space_ptr;
    block_header* end_mark;
    static int allocated_once = 0;
  
    if (0 != allocated_once) {
        fprintf(stderr, 
        "Error:mem.c: Init_Mem has allocated space during a previous call\n");
        return -1;
    }
    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion 
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    alloc_size = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, 
                    fd, 0);
    if (MAP_FAILED == space_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }
  
    allocated_once = 1;

    // for double word alignment and end mark
    alloc_size -= 8;

    // To begin with there is only one big free block
    // initialize heap so that start block meets 
    // double word alignement requirement
    start_block = (block_header*) space_ptr + 1;
    end_mark = (block_header*)((void*)start_block + alloc_size);
  
    // Setting up the header
    start_block->size_status = alloc_size;

    // Marking the previous block as used
    start_block->size_status += 2;

    // Setting up the end mark and marking it as used
    end_mark->size_status = 1;

    // Setting up the footer
    block_header *footer = (block_header*) ((char*)start_block + alloc_size - 4);
    footer->size_status = alloc_size;
  
    return 0;
}         
                 
/* 
 * Function to be used for DEBUGGING to help you visualize your heap structure.
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block 
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block as stored in the block header
 */                     
void Dump_Mem() {         
    int counter;
    char status[5];
    char p_status[5];
    char *t_begin = NULL;
    char *t_end = NULL;
    int t_size;

    block_header *current = start_block;
    counter = 1;

    int used_size = 0;
    int free_size = 0;
    int is_used = -1;

    fprintf(stdout, "************************************Block list***\
                    ********************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, "-------------------------------------------------\
                    --------------------------------\n");
  
    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;
    
        if (t_size & 1) {
            // LSB = 1 => used block
            strcpy(status, "used");
            is_used = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "Free");
            is_used = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "used");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "Free");
        }

        if (is_used) 
            used_size += t_size;
        else 
            free_size += t_size;

        t_end = t_begin + t_size - 1;
    
        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n", counter, status, 
        p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);
    
        current = (block_header*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, "---------------------------------------------------\
                    ------------------------------\n");
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fprintf(stdout, "Total used size = %d\n", used_size);
    fprintf(stdout, "Total free size = %d\n", free_size);
    fprintf(stdout, "Total size = %d\n", used_size + free_size);
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fflush(stdout);

    return;
}         
