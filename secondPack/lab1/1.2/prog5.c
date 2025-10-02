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
    printf("mythread %d: Hello from mythread!\n", gettid());
    return NULL;
}

int main() {
	pthread_t tid;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int is_err_create;
    int count = 0;

	printf("main %d: Hello from main!\n", gettid());
    while(TRUE){
        is_err_create = pthread_create(&tid, &attr, mythread, NULL);
        if (is_err_create != SUCCESS) {
            pthread_attr_destroy(&attr);
            perror("bad work with pthread_create().\n");
            printf("pthread_create returned 'SUCCESS' %d times\n", count);
            return ERROR;
        }
        count++;
    }

    pthread_attr_destroy(&attr);
	return 0;
}

