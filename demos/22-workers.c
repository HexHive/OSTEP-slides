#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define BUFSIZE 4
#define NRITEMS 5000

pthread_mutex_t box[BUFSIZE] = { PTHREAD_MUTEX_INITIALIZER };
int buf[BUFSIZE] = { 0 };

void *worker(void *arg) {
	int tid = (int)arg;
	int nritems = NRITEMS;
	printf("Worker %d starting, need to pass on %d items.\n", tid, nritems);
	pthread_mutex_t *l1, *l2;
	l1 = &(box[tid]);
	l2 = &(box[(tid+1) % BUFSIZE]);
	if (tid == BUFSIZE - 1) {
		pthread_mutex_t *tmp = l2;
		l2 = l1;
		l1 = tmp;
	}
	while (nritems != 0) {
		pthread_mutex_lock(l1);
		pthread_mutex_lock(l2);
		if (buf[tid] > 0) {
			buf[(tid+1) % BUFSIZE]++;
			buf[tid]--;
			nritems--;
		}
		pthread_mutex_unlock(l2);
		pthread_mutex_unlock(l1);
	}
	return NULL;
}

int main(int argc, char *argv[]) {
	printf("main thread (%s, %d)\n", argv[0], argc);
	pthread_t p[BUFSIZE];

	for (int i = 0; i < BUFSIZE; i++)
		pthread_create(&p[i], NULL, &worker, (void*)(long)i);
	buf[0] = 25;

	for (int i = 0; i < BUFSIZE; i++)
		pthread_join(p[i], NULL);
	for (int i = 0; i < BUFSIZE; i++)
		printf("%d ", buf[i]);
	printf("\nmain ends\n");
	return 0;
}
