#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/**
 * Heap area, provided by the OS
 */
#define HEAPSIZE 1024*1024
char heap[HEAPSIZE];

/****************************
 * Allocator implementation *
 ****************************/

/**
 * Data structure for free memory objects.
 * The object size is first so that allocated objects can use the same slot.
 */
struct mitem {
  size_t size;
  struct mitem *next;
};


/**
 * Global pointer to first free memory object.
 */
struct mitem *first = (struct mitem*)heap;


/**
 * Initialize heap memory allocator
 */
void init() {
  first->next = NULL;
  first->size = HEAPSIZE - sizeof(struct mitem);
}


/**
 * alloc a memory chunk of size bytes
 */
void *xalloc(size_t size) {
  struct mitem *ptr = first, *prev = NULL, *new;
  if (size == 0) return NULL;
  printf("alloc %ld bytes", size);
  // round up to nearest 16 bytes (at 15 and round down)
  size = (size + sizeof(size_t) + sizeof(struct mitem) - 1) 
           & (long)(-sizeof(struct mitem));
  printf(" (%ld effectively used)", size);

  // find empty slot or return NULL
  while (ptr->size < size) {
    if (ptr->next == NULL) {
      printf(" not enough memory, bailing.\n");
      return NULL;
    }
    prev = ptr;
    ptr = ptr->next;
  }
  printf(" at ptr %p\n", (void*)ptr);

  // split slot
  if (ptr->size > size) {
    new = (struct mitem*)(((char*)(ptr)+size));
    new->next = ptr->next;
    new->size = ptr->size - size;
    ptr->size = size;
  } else {
    // perfect fit, just return next pointer
    new = ptr->next;
  }

  // reconnect list
  if (prev == NULL)
    first = new;
  else
    prev->next = new;

  // return adjusted pointer
  return (char*)(ptr)+sizeof(size_t);
}


/**
 * Free an object, read size from LHS.
 */
void xfree(void *vptr) {
  // get pointer to size
  struct mitem *ptr = (struct mitem*)(((char*)vptr)-sizeof(size_t));
  printf("free %p (metadata at %p), %ld\n", vptr, (void*)ptr, ptr->size);
  // link back to list
  ptr->next = first;
  first = ptr;
}


/************************************
 * Some simple test vectors in main *
 ************************************/
int main(int argc __attribute__((unused)), char *argtv[] __attribute__((unused))) {
  char *ptrs[64] = {0};
  init();
  // simple test
  for (int i = 0; i < 64; i++) {
    ptrs[i] = (char*)xalloc(i);
    for (int j = 0; j < i; j++)
      ptrs[i][j] = 0x41;
  }
  for (int i = 63; i >= 0; i--) {
    xfree(ptrs[i]);
    ptrs[i] = 0;
  }

  // fuzz a bit
  for (int i=0; i<1000000; i++) {
    int item = rand()%64;
    if (ptrs[item] == NULL) {
        int size = rand()%256;
        ptrs[item] = xalloc(size);
        if (ptrs[item] != NULL && size >= 1) {
          ptrs[item][0] = (unsigned char)size;
          memset(ptrs[item]+1, 0x41, size-1);
        }
    } else {
        xfree(ptrs[item]);
        ptrs[item] = NULL;
    }
  }
  
  return 0;
}
