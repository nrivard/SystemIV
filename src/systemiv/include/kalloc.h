#pragma once

#define PGSIZE          4096  // 4k page size
#define PGROUNDUP(mem)  ((mem+(PGSIZE-1)) & ~(PGSIZE-1))

// initializes all possible pages of memory
void kinit(void);

// frees a single page of memory
void kfree(void *);

// allocates a single page of memory
// returns NULL if no more pages are available
void *kalloc(void);
