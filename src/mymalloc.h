#ifndef _MYMALLOC_H
#define _MYMALLOC_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Node{
	int size;
	struct Node *next;
} *Node;

void *my_malloc(size_t);
void my_free(void *);
void *free_list_begin();
void *free_list_next(void *);
void coalesce_free_list();

#ifdef __cplusplus
}  /* extern C */
#endif

#endif
