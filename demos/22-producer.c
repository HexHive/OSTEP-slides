#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

/**
 * Example of 2 producers / 1 consumer
 */

#define BUFSIZE 2
#define NUMITEMS 8

void put(unsigned int);
unsigned int get();

unsigned int buffer[BUFSIZE] = { 0 };

/* semaphores handling *concurrent* access to buffer */
sem_t csem, psem;

/* mutex handling mutual exclusive access to ppos */
pthread_mutex_t pmutex = PTHREAD_MUTEX_INITIALIZER;

unsigned int cpos = 0, ppos = 0;

void *producer(void *arg) {
	unsigned int max = (unsigned int)arg;
	for (unsigned int i = 0; i < max; i++) {
		printf("Produced %d\n", i);
		put(i);
	}
	return NULL;
}

void *consumer(void *arg) {
	unsigned int max = (unsigned int)arg;
	for (unsigned int i = 0; i < max; i++) {
		printf("Consumed %d\n", get(i));
	}
	return NULL;
}

void put(unsigned int val) {
	unsigned int mypos;

	/* we wait until there is buffer space available */
	sem_wait(&psem);

	/* ppos is shared between all producers */
	pthread_mutex_lock(&pmutex);
	mypos = ppos;
	ppos = (ppos + 1) % BUFSIZE;

	/* store information in buffer */
	buffer[mypos] = val;
	pthread_mutex_unlock(&pmutex);
	sem_post(&csem);
}

unsigned int get() {
	sem_wait(&csem);
	unsigned long val = buffer[cpos];
	cpos = (cpos + 1) % BUFSIZE;
	sem_post(&psem);
	return val;
}

int main(int argc, char *argv[]) {
	printf("main thread (%s, %d)\n", argv[0], argc);
	pthread_t p1, p2, c;

	// BUFSIZE items are available for producer to create
	sem_init(&psem, 0, BUFSIZE);
	// 0 items are available for consumer
	sem_init(&csem, 0, 0);

	pthread_create(&p1, NULL, &producer, (void*)NUMITEMS);
	pthread_create(&p2, NULL, &producer, (void*)NUMITEMS);
	pthread_create(&c, NULL, &consumer, (void*)(2*NUMITEMS));
	
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	pthread_join(c, NULL);
	printf("main ends\n");
	return 0;
}
