#ifndef _MALLOC_H_
#define _MALLOC_H_
#include <stddef.h>
#include <stdbool.h>

#define MAGIC        123456  
#define MAX_BLOCKS   32
#define MIN_SIZE     256

#define BIG_BLOCK    33554432 
#define MEDIUM_BLOCK 1048576
#define SMALL_BLOCK  16384

struct region {
	size_t size;
	size_t magic;
	bool free;
	struct region *next;
};

struct block {
	size_t size;
	struct region *head;
	struct block *next;
};

int
check_pointer(void *p);

void
coalescing(void);

void
libero_bloque(void);

void *malloc(size_t size);

void free(void *ptr);

void *calloc(size_t nmemb, size_t size);

void *realloc(void *ptr, size_t size);

struct region * 
splitHeap(size_t size, struct region * next);

void
change_amount_block(size_t block_size);
void
removeAmountBlocks(size_t block_size);

#endif // _MALLOC_H_
