#define _DEFAULT_SOURCE

#include <inttypes.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "malloc.h"
#include "printfmt.h"
#include <sys/mman.h>
#include <string.h>
#include <limits.h>

#define ALIGN4(s) (((((s) -1) >> 2) << 2) + 4)
#define REGION2PTR(r) ((r) + 1)
#define PTR2REGION(ptr) ((struct region *) (ptr) -1)

/* returns 0 if pointer is invalid*/
int
check_pointer(void *p)
{
	size_t pg_size = sysconf(_SC_PAGESIZE);
	void *pg_start = (void *) ((((size_t) p) / pg_size) * pg_size);
	return msync(pg_start, pg_size, MS_ASYNC) == 0;
}

struct block *block_list = NULL;

int amount_of_blocks = 0;
int amount_of_small_blocks = 0;
int amount_of_media_blocks = 0;
int amount_of_big_blocks = 0;
int amount_of_mallocs = 0;
int amount_of_frees = 0;
int requested_memory = 0;
int amount_of_splits = 0;
int amount_of_coalesing = 0;

/*
static void
print_statistics(void)
{
        printfmt("requested: %d\n", requested_memory);
        printfmt("mallocs:   %d\n", amount_of_mallocs);
        printfmt("frees:     %d\n", amount_of_frees);
        printfmt("blocks:    %d\n", amount_of_blocks);
        printfmt("blocks:    %d\n", block_list->size);
}*/

// finds the next free region
// that holds the requested size
//

static struct region *
find_free_region(size_t size)
{
	if (!block_list)
		return NULL;

	struct region *node = NULL;
	struct block *block = block_list;

	bool flag = false;

	while (block) {
		struct region *it = block->head;
		while (it) {  // Dado un nodo
			if (it->free &&
			    (it->size >=
			     size)) {  // Si el nodo está liberado y puede almacenar la región:
#ifdef FIRST_FIT
				node = it;  // Me guardo el nodo y termino el ciclo
				flag = true;
				break;
#endif
#ifdef BEST_FIT
				if (!node || (it->size < node
				                                 ->size))  //  Si es un mejor candidato para guardar una región, o no hay candidato:
					node = it;  // Guardo el nodo
#endif
			}
			it = it->next;  // Itero al siguiente nodo
		}
		if (flag)
			break;
		block = block->next;  // Itero al siguiente bloque
	}
	return node;
}

/// prev != NULL & size != NULL
struct region *
splitHeap(size_t size, struct region *prev)
{
	if ((prev->size - size) < sizeof(struct region)) {
		prev->free = false;
		return prev;
	}

	amount_of_splits++;

	struct region *free_region =
	        (struct region *) (((char *) (prev + 1)) + size);

	free_region->free = true;
	free_region->next = prev->next;
	free_region->magic = MAGIC;
	free_region->size = prev->size - (size + (sizeof(struct region)));

	prev->next = free_region;
	prev->free = false;
	prev->size = size;

	return prev;
}

void
change_amount_block(size_t block_size)
{
	if (block_size == SMALL_BLOCK)
		amount_of_small_blocks++;
	if (block_size == MEDIUM_BLOCK)
		amount_of_media_blocks++;
	if (block_size == BIG_BLOCK)
		amount_of_big_blocks++;
};

void
removeAmountBlocks(size_t block_size)
{
	if (block_size == SMALL_BLOCK)
		amount_of_small_blocks--;
	if (block_size == MEDIUM_BLOCK)
		amount_of_media_blocks--;
	if (block_size == BIG_BLOCK)
		amount_of_big_blocks--;
};

static struct region *
grow_heap(size_t size)
{
	size_t block_size = SMALL_BLOCK;
	if (size > block_size) {
		block_size = MEDIUM_BLOCK;
		if (size > block_size)
			block_size = BIG_BLOCK;
	}
	change_amount_block(block_size);
	struct block *new_block =
	        mmap(NULL,
	             block_size + sizeof(struct region) + sizeof(struct block),
	             PROT_READ | PROT_WRITE,
	             MAP_ANON | MAP_PRIVATE,
	             -1,
	             0);

	struct region *region = (struct region *) (new_block + 1);

	new_block->size = block_size;
	new_block->head = region;
	new_block->next = NULL;

	if (!block_list)
		block_list = new_block;
	else {
		struct block *block_aux = block_list;
		while (block_aux->next) {
			block_aux = block_aux->next;
		}
		block_aux->next = new_block;
	}

	amount_of_blocks++;

	region->size = block_size;
	region->next = NULL;
	region->free = true;
	region->magic = MAGIC;

	return region;
}

void *
malloc(size_t size)
{
	if (size > BIG_BLOCK || size <= 0) {
		errno = ENOMEM;
		return NULL;
	}

	if (size < MIN_SIZE)
		size = MIN_SIZE;

	// aligns to multiple of 4 bytes
	size = ALIGN4(size);

	// updates statistics
	amount_of_mallocs++;
	requested_memory += size;

	struct region *next = find_free_region(size);

	if (!next) {
		next = grow_heap(size);
	}
	next = splitHeap(size, next);

	return REGION2PTR(next);
}

void
free(void *ptr)
{
	if (!ptr || !check_pointer(ptr))
		return;

	struct region *curr = PTR2REGION(ptr);

	if (curr->magic != MAGIC || curr->free)
		return;
	amount_of_frees++;
	requested_memory -= curr->size;

	curr->free = true;

	coalescing();
	libero_bloque();
}

void
coalescing(void)
{
	struct block *block = block_list;
	struct region *region;
	while (block) {
		region = block->head;
		while (region && region->next) {
			if (region->free && region->next->free) {
				amount_of_coalesing++;
				region->next->magic = 0;
				region->size += (region->next->size +
				                 sizeof(struct region));
				region->next = region->next->next;
			} else {
				region = region->next;
			}
		}
		block = block->next;
	}
}
void
libero_bloque(void)
{
	int tamanio = sizeof(struct region) + sizeof(struct block);

	// para primer bloque
	struct block *block = block_list;
	if (block->head->free && !block->head->next) {
		tamanio += block->size;
		block_list = block->next;
		removeAmountBlocks(block->size);
		munmap(block, tamanio);
		amount_of_blocks--;
		return;
	}

	struct block *block_ant = block;
	block = block_ant->next;

	while (block) {
		if (block->head->free && !block->head->next) {
			tamanio += block->size;
			block_ant->next = block->next;
			removeAmountBlocks(block->size);
			munmap(block, tamanio);
			amount_of_blocks--;
			return;
		}
		block_ant = block;
		block = block_ant->next;
	}
}

void *
calloc(size_t nmemb, size_t size)
{
	if (nmemb == 0 || size == 0 || ((int) nmemb < 0 && (int) size < 1) ||
	    (nmemb >
	     (INT_MAX / size))) {  // Si nmemb o size == 0 entonces retorno null
		return malloc(0);
	}

	char *next;
	next = malloc(size * nmemb);

	if (!next) {
		return malloc(0);
	}

	memset(next, 0, ALIGN4(size * nmemb));  // Inicializo memoria en 0

	return next;
}

void *
realloc(void *ptr, size_t size)
{
	if (!check_pointer(ptr)) {
		errno = ENOMEM;
		return NULL;
	}
	if (ptr == NULL) {
		errno = ENOMEM;
		return malloc(size);
	}
	if (size == 0) {
		free(ptr);
		return NULL;
	}

	struct region *region_ = PTR2REGION(ptr);

	if (region_->magic != MAGIC) {
		errno = ENOMEM;
		return NULL;
	}

	if (region_->size >= size) {
		return REGION2PTR(region_);
	}

	void *new_ptr_ = malloc(size);
	struct region *new_ptr = PTR2REGION(new_ptr_);
	if (!new_ptr) {
		errno = ENOMEM;
		return NULL;
	}
	memcpy(new_ptr, region_, region_->size);
	free(region_);
	return REGION2PTR(new_ptr);
}