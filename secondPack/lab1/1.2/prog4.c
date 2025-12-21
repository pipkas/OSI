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
    pthread_detach(pthread_self());
    printf("mythread %d\n", gettid());
    return NULL;
}

int main() {
	pthread_t tid;
    int is_err_create;
    int count = 0;

	printf("main %d: Hello from main!\n", gettid());
    while(TRUE){
        is_err_create = pthread_create(&tid, NULL, mythread, NULL);
        if (is_err_create != SUCCESS) {
            perror("bad work with pthread_create().\n");
            sleep(1);
            printf("pthread_create() returned %d times SUCCESS\n", count);
            return ERROR;
        }
        count++;
    }

	return 0;
}

