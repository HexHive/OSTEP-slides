#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

bool done = false;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

void thr_exit() {
	pthread_mutex_lock(&m);
	done = true;
	pthread_cond_signal(&c);
	pthread_mutex_unlock(&m);
}

void thr_join() {
	pthread_mutex_lock(&m);
	while (!done)
		pthread_cond_wait(&c, &m);
	pthread_mutex_unlock(&m);
}

void *runner(void *arg) {
	printf("child: %s\n", (char*)arg);
	sleep(1);
	thr_exit();
	return NULL;
}

int main(int argc, char *argv[]) {
	printf("main thread (%s, %d)\n", argv[0], argc);
	pthread_t p;
	pthread_create(&p, NULL, &runner, "1");
	thr_join();
	printf("main ends\n");
	return 0;
}
