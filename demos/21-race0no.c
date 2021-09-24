#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITER 10000000

unsigned long counter = 0;

__thread unsigned int tid = -1;

void *incer(void *ltid) {
  tid = (unsigned int)ltid;
  for (unsigned long i = 0; i < ITER; ++i) {
    counter = counter + 1;
  }
  return NULL;
}


/**
 * Simple main that launches tests for each
 * type of locking mechanism.
 */
int main(int argc __attribute__((unused)), 
         char* argv[] __attribute__((unused))) {

  pthread_t t1, t2;
  clock_t start, end;

  printf("Launching test 'no locking'\n");
  counter = 0;

  start = clock();

  /* create worker threads */
  if (pthread_create(&t1, NULL, incer, (void*)0) != 0) exit(-1);
  if (pthread_create(&t2, NULL, incer, (void*)1) != 0) exit(-1);

  /* work is done, merge threads */
  if (pthread_join(t1, NULL) != 0) exit(-1);
  if (pthread_join(t2, NULL) != 0) exit(-1);

  end = clock();

  printf("Counter: %lu (expected: %lu), elapsed: %fs\n", counter,
          ((unsigned long)ITER)*2, (double)(end-start)/CLOCKS_PER_SEC);

  return 0;
}
