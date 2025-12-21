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
#define MAX_MAP_COUNT 65530
#define TEST_TIME_DURATION 1000

void *mythread(void *arg) {
    (void)arg;
    printf("mythread %d:\n", gettid());
    return NULL;
}

int main() {
	pthread_t tid[MAX_MAP_COUNT];
    int is_err_create;
    int count = 0;

	printf("main %d: Hello from main!\n", gettid());
    while(TRUE){
        is_err_create = pthread_create(&(tid[count]), NULL, mythread, NULL);
        if (is_err_create != SUCCESS) {
            perror("bad work with pthread_create().\n");
            break;
        }
        count++;
        usleep(TEST_TIME_DURATION);
    }

    for (int i = 0; i < count; i++){
        int is_err_join = pthread_join(tid[i], NULL);
        if (is_err_join != SUCCESS) {
            perror("bad work with pthread_join().\n");
            printf("pthread_create() returned %d times SUCCESS\n", count);
            return ERROR;
        }
    }
    printf("pthread_create() returned %d times SUCCESS\n", count);

    if(is_err_create != SUCCESS){
        return ERROR;
    }

	return 0;
}

