#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h>
#define SUCCESS 0
#define ERROR -1

void *mythread(void *arg) {
	(void)arg;
	printf("mythread %d: Hello from mythread!\n", gettid());
    char* greeting = strdup("Hello world");
    if (greeting == NULL){
        perror("Error with 'strdup' in 'mythread' function\n");
        return NULL;
    }
	return (void*)greeting;
}

int main() {
	pthread_t tid;
	void* thread_ret;

	printf("main %d: Hello from main!\n", gettid());

	int is_err = pthread_create(&tid, NULL, mythread, NULL);
	if (is_err != SUCCESS) {
		perror("bad work with pthread_create().\n");
		return ERROR;
	}
	is_err = pthread_join(tid, &thread_ret);
	if (is_err != SUCCESS) {
		perror("bad work with pthread_join().\n");
		return ERROR;
	}
    if (thread_ret == NULL){
        return ERROR;
    }

	printf("mythread returned: %s\n", (char*)thread_ret);
    free(thread_ret);

	return 0;
}

