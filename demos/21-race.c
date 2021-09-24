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
  /* special field for pthread mutex */
  pthread_mutex_t pmutex;
};


void nop(struct lock *);
void acquire_sw(struct lock *);
void release_sw(struct lock *);
void acquire_peterson(struct lock *);
void release_peterson(struct lock *);
void acquire_tas(struct lock *);
void release_tas(struct lock *);
void acquire_cas(struct lock *);
void release_cas(struct lock *);
void acquire_pth(struct lock *);
void release_pth(struct lock *);

unsigned long counter = 0;

struct lock lock1 = { false, { false, false}, 0, {{0, 0, 0, 0, 0, 0, 0, {0, 0}}} };

void (*acquire)(struct lock *) = &nop;
void (*release)(struct lock *) = &nop;

__thread unsigned int tid = -1;

void *incer(void *ltid) {
  tid = (unsigned int)ltid;
  for (unsigned long i = 0; i < ITER; ++i) {
    acquire(&lock1);
    counter = counter + 1;
    release(&lock1);
  }
  // printf("%s is done", (char*)arg);
  return NULL;
}

void launch_test(const char *str) {
  pthread_t t1, t2;
  clock_t start, end;

  printf("Launching test '%s'\n", str);
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
}


/**
 * No locking
 */
void nop(struct lock *l __attribute__((unused))) {
}


/**
 * Software locking, no hardware support
 */
void acquire_sw(struct lock *l) {
  while (l->taken); /* spin until we grab the lock */
  l->taken = true;
}

void release_sw(struct lock *l) {
  l->taken = false;
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
 * Test and Set
 */
bool tas(bool *addr, bool val) {
  /* This code is equivalent to the intrinsic:
  bool old;
  __asm__ volatile("lock; xchgb %0, %1" :
               "+m" (*addr), "=a" (old) :
               "1" (val) : "cc");
  return old; */
  return __sync_lock_test_and_set(addr, val);
}

void acquire_tas(struct lock *l) {
  while (tas(&(l->taken), true) == true); /* spin */
}

void release_tas(struct lock *l) {
  l->taken = false;
}


/**
 * Compare and Swap
 */
void acquire_cas(struct lock *l) {
  /* instruction on x86: lock; cmpxchgb */
  while (__sync_bool_compare_and_swap(&(l->taken), false, true) == false); /* spin */
}

void release_cas(struct lock *l) {
  l->taken = false;
}


/**
 * pthread mutex
 */
void acquire_pth(struct lock *l) {
  pthread_mutex_lock(&(l->pmutex));
}

void release_pth(struct lock *l) {
  pthread_mutex_unlock(&(l->pmutex));

}


/**
 * Simple main that launches tests for each
 * type of locking mechanism.
 */
int main(int argc __attribute__((unused)), 
         char* argv[] __attribute__((unused))) {

  launch_test("No locking");

  acquire = &acquire_sw;
  release = &release_sw;
  launch_test("SW only, no HW support");

  acquire = &acquire_peterson;
  release = &release_peterson;
  launch_test("Peterson's locking");

  acquire = &acquire_tas;
  release = &release_tas;
  launch_test("Test and Set");

  acquire = &acquire_cas;
  release = &release_cas;
  launch_test("Compare and Swap");

  /* prepare mutex */
  if (pthread_mutex_init(&(lock1.pmutex), NULL) != 0) exit(-1);
  acquire = &acquire_pth;
  release = &release_pth;
  launch_test("pthread mutex");

  return 0;
}

