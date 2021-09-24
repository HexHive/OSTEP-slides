#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITER 10000000

struct lock {
  /* generic lock variable */
  bool taken;
  /* special fields for peterson */
  bool peterson[2];
  unsigned int turn;
};

void acquire_peterson(struct lock *);
void release_peterson(struct lock *);

unsigned long counter = 0;

__thread unsigned int tid = -1;

struct lock lock1 = { false, {false, false}, 0 };

void *incer(void *ltid) {
  tid = (unsigned int)ltid;
  for (unsigned long i = 0; i < ITER; ++i) {
    acquire_peterson(&lock1);
    counter = counter + 1;
    release_peterson(&lock1);
  }
  return NULL;
}


/**
 * Peterson's locking mechanism, assumes atomic hardware instr and no caching
 */
void acquire_peterson(struct lock *l) {
  l->peterson[tid] = true;
  l->turn = 1-tid;
  while (l->peterson[1-tid] && l->turn == 1-tid); /* wait */
}

void release_peterson(struct lock *l) {
  l->peterson[tid] = false;
}

/**
 * Simple main that launches tests for each
 * type of locking mechanism.
 */
int main(int argc __attribute__((unused)), 
         char* argv[] __attribute__((unused))) {

  pthread_t t1, t2;
  clock_t start, end;

  printf("Launching test 'Gary Peterson locking'\n");
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

