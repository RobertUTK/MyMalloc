#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mymalloc.h"

#define MIN_SIZE 8192
#define PTR_OFFSET 8
#define EIGHT 8

Node  free_list_head = NULL;

static int countList();
static int cmpPtrs(const void *, const void *);

/*my_malloc: A buffered interface to sbrk() that doles out heap memory to a program that calls it.
 *Param: size, the size of the memory needed
 *Returns: a void pointer to the memory address allocated.
 */
extern void*
my_malloc(size_t size)
{
    void *ptr;
    Node node, prev, next;
    int remainder;
    size_t remaining;

    remainder = size % EIGHT;
    if(remainder != 0) size += EIGHT - remainder;
    size += EIGHT;

    if(free_list_head == NULL){
        if(size < MIN_SIZE){
            ptr = sbrk(MIN_SIZE);
            node = (Node) ptr;
            node->size = size;

            remaining = MIN_SIZE - size;
            node = (Node) (ptr+size);
            node->size = remaining;
            node->next = NULL;
            free_list_head = node;
            return ptr + PTR_OFFSET;
        }
        else{
            ptr = sbrk(size);
            node = (Node) ptr;
            node->size = size;
            return ptr + PTR_OFFSET;
        }
    }
    else{
        node = free_list_head;
        prev = node;
        while(node != NULL){
            if(node->size >= size){
                if((node->size - size) < 16){
                    if(node == free_list_head) free_list_head = node->next;
                    else prev->next = node->next;
                    ptr = (void*) node;
                    return ptr + PTR_OFFSET;
                }
                else{
                    ptr = (void*) node;
                    next = (Node) (ptr + size);
                    next->size = node->size - size;
                    node->size = size;
                    next->next = node->next;
                    if(node == free_list_head) free_list_head = next;
                    else prev->next = next;
                    return ptr + PTR_OFFSET;
                }
            }
            prev = node;
            node = node->next;
        }
        if(size < MIN_SIZE){
            ptr = sbrk(MIN_SIZE);
            node = (Node) ptr;
            node->size = size;

            remaining = MIN_SIZE - size;
            node = (Node) (ptr+size);
            node->size = remaining;
            node->next = free_list_head;
            free_list_head = node;
            return ptr + PTR_OFFSET;
        }
        else{
            ptr = sbrk(size);
            node = (Node) ptr;
            node->size = size;
            return ptr + PTR_OFFSET;
        }
    }
}

/*my_free: Returns a chunk of memory to the free list.
 *Param: ptr, a pointer to the memory address allocated
 */
void
my_free(void *ptr)
{
    Node node;
    node = (Node) (ptr - PTR_OFFSET);
    node->next = free_list_head;
    free_list_head = node;
}

/*free_list_begin: returns a pointer to the first node of the free list.
 *Returns: a pointer to the first node or NULL if empty
 */
void *
free_list_begin()
{
    return free_list_head;
}

/*free_list_next: returns a pointer to the next node in the list from a given node
 *Param: n, a node in the list
 *Returns: a pointer to the next node in the list or NULL if n is the last node
 */
void *
free_list_next(void *n)
{
    Node node = (Node) n;
    if(node != NULL) return node->next;
    else return NULL;
}

/*coalesce_free_list: Sorts the list and combines all adjacent entries
 *Post-Con: All adjacent nodes will be combined
 */
void
coalesce_free_list()
{
    int count, i;
    void *ptr, *nPtr;
    Node cur; 
    Node *sortList;

    count = countList();

    sortList = (Node*) malloc(sizeof(Node)*count);
    cur = free_list_head;
    for(i = 0; i < count; i++){
        sortList[i] = cur;
        cur = cur->next;
    }
    qsort(sortList, count, sizeof(Node), cmpPtrs);

    free_list_head = sortList[0];
    cur = free_list_head;
    for(i = 1; i < count; i++){
        cur->next = sortList[i];
        cur = cur->next;
    }
    cur->next = NULL;

    cur = free_list_head;
    while(cur != NULL){
        ptr = (void*) cur;
        nPtr = (void*) cur->next;
        if((ptr + cur->size) == nPtr){
            cur->size += cur->next->size;
            cur->next = cur->next->next;
        }
        else cur = cur->next;
    }

    free(sortList);
}

/*countList: Counts the number of nodes in the list
 *Returns: the number of nodes in the list
 */
static int
countList()
{
    int count = 0;
    Node n;
    n = free_list_head;
    while(n != NULL){
        count++;
        n = n->next;
    }
    return count;
}

/*cmpPtrs: the compare function for Qsort that compares 2 memory addresses
 *Param: p1, a void ptr to a Node*
 *		 p2, a void ptr to a Node*
 *Returns: -1 if n1 is less than n2 else 1
 */		   
static int
cmpPtrs(const void *p1, const void *p2)
{
    Node* n1 = (Node*) p1;
    Node* n2 = (Node*) p2;
    
    if(*n1 < *n2){
        return -1;
    }
    else{ 
        return 1;
    }
}
