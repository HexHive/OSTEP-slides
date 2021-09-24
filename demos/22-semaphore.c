#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/**
 * Same example as 22-producer.c but this time we
 * implement the semaphore ourselves!
 */

typedef struct {
	int value;
	pthread_mutex_t lock;
	pthread_cond_t cond;
} sem_t;

void sem_init(sem_t *s, int val) {
	s->value = val;
	pthread_mutex_init(&(s->lock), NULL);
	pthread_cond_init(&(s->cond), NULL);
}

void sem_wait(sem_t *s) {
	pthread_mutex_lock(&(s->lock));
	while (s->value <= 0)
		pthread_cond_wait(&(s->cond), &(s->lock));
	s->value--;
	pthread_mutex_unlock(&(s->lock));
}

void sem_post(sem_t *s) {
	pthread_mutex_lock(&(s->lock));
	s->value++;
	pthread_cond_signal(&(s->cond));
	pthread_mutex_unlock(&(s->lock));
}

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
	sem_init(&psem, BUFSIZE);
	// 0 items are available for consumer
	sem_init(&csem, 0);

	pthread_create(&p1, NULL, &producer, (void*)NUMITEMS);
	pthread_create(&p2, NULL, &producer, (void*)NUMITEMS);
	pthread_create(&c, NULL, &consumer, (void*)(2*NUMITEMS));
	
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	pthread_join(c, NULL);
	printf("main ends\n");
	return 0;
}
