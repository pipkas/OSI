#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#define SUCCESS 0
#define ERROR -1

void *mythread(void *arg) {
	(void)arg;
	printf("mythread [%d %d %d]: Hello from mythread!\n", getpid(), getppid(), gettid());
	return NULL;
}

int main() {
	pthread_t tid;

	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

	int is_err = pthread_create(&tid, NULL, mythread, NULL);
	if (is_err != SUCCESS) {
		perror("bad work with pthread_create().\n");
		return ERROR;
	}
	is_err = pthread_join(tid, NULL);
	if (is_err != SUCCESS) {
		perror("bad work with pthread_join().\n");
		return ERROR;
	}

	return 0;
}

