#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#define SUCCESS 0
#define ERROR -1
#define TRUE 1

void *mythread(void *arg) {
    (void)arg;
    printf("mythread %d\n", gettid());
    return NULL;
}

int main() {
	pthread_t tid;
    pthread_attr_t attr;

    int is_err = pthread_attr_init(&attr);
    if (is_err != SUCCESS) {
		perror("bad work with pthread_attr_init().\n");
		return ERROR;
	}
    is_err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (is_err != SUCCESS) {
        pthread_attr_destroy(&attr);
		perror("bad work with pthread_attr_setdetachstate().\n");
		return ERROR;
	}

	printf("main %d: Hello from main!\n", gettid());
    int count = 0;
    while(TRUE){
        is_err = pthread_create(&tid, &attr, mythread, NULL);
        if (is_err != SUCCESS) {
            pthread_attr_destroy(&attr);
            perror("bad work with pthread_create().\n");
            sleep(1);
            printf("pthread_create() returned %d times SUCCESS\n", count);
            return ERROR;
        }
        count++;
    }

    pthread_attr_destroy(&attr);
	return 0;
}

