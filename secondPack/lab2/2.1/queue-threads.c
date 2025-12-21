#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <pthread.h>
#include <sched.h>

#include "queue.h"

#define RED "\033[41m"
#define NOCOLOR "\033[0m"
#define TRUE 1
#define ERROR -1
#define SUCCESS 0
#define QUEUE_MAX_COUNT 100000

void set_cpu(int n) {
	int err;
	cpu_set_t cpuset;
	pthread_t tid = pthread_self();

	CPU_ZERO(&cpuset);
	CPU_SET(n, &cpuset);

	err = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
	if (err != SUCCESS) {
		printf("set_cpu: pthread_setaffinity failed for cpu %d\n", n);
		return;
	}

	printf("set_cpu: set cpu %d\n", n);
}

void *reader(void *arg) {
	int expected = 0;
	queue_t *q = (queue_t *)arg;
	printf("reader [%d %d %d]\n", getpid(), getppid(), gettid());

	set_cpu(1);

	while (TRUE) {
		int val = -1;
		int ok = queue_get(q, &val);
		if (!ok)
			continue;

		if (expected != val)
			printf(RED"ERROR: get value is %d but expected - %d" NOCOLOR "\n", val, expected);

		expected = val + 1;
	}

	return NULL;
}

void *writer(void *arg) {
	int i = 0;
	queue_t *q = (queue_t *)arg;
	printf("writer [%d %d %d]\n", getpid(), getppid(), gettid());

	set_cpu(1);

	while (TRUE) {
		int ok = queue_add(q, i);
		if (!ok)
			continue;
		i++;
	}

	return NULL;
}

int main() {
	pthread_t tid1,tid2;
	queue_t *q;
	int err;

	printf("main [%d %d %d]\n", getpid(), getppid(), gettid());

	q = queue_init(QUEUE_MAX_COUNT);

	err = pthread_create(&tid1, NULL, reader, q);
	if (err != SUCCESS) {
		perror("main: pthread_create() failed\n");
		return ERROR;
	}

	err = pthread_create(&tid2, NULL, writer, q);
	if (err != SUCCESS) {
		perror("main: pthread_create() failed\n");
		return ERROR;
	}

	err = pthread_join(tid1, NULL);
	if (err != SUCCESS) {
		perror("main: pthread_join() failed\n");
		return ERROR;
	}

	err = pthread_join(tid2, NULL);
	if (err != SUCCESS) {
		perror("main: pthread_join() failed\n");
		return ERROR;
	}

	queue_destroy(q);

	return 0;
}
