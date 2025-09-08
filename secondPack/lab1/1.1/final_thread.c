#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#define THREADS_COUNT 5 
#define SUCCESS 0
#define ERROR -1
int global = 5;
pthread_mutex_t printf_mutex = PTHREAD_MUTEX_INITIALIZER;

void *mythread(void *arg) {
	(void)arg;
    const int local_const = 7;
    int local = 2;
    static int local_static = 8;

    pthread_mutex_lock(&printf_mutex);
    if (local_static == 8){
        printf("Initial values: local = 2, global = 5\n\n");
        global++;
        local++;
        local_static++;
    }
    printf("pid: %d, ppid: %d, tid: %d\n", getpid(), getppid(), gettid());
    printf("Values: local = %d, global = %d\n", local, global);
    printf("addresses:  local: %p, local const: %p, local static: %p, global: %p\n\n",\
        (void*)(&local), (void*)(&local_const), (void*)(&local_static), (void*)(&global));
    pthread_mutex_unlock(&printf_mutex);

    long unsigned* self_id = (long unsigned*)malloc(sizeof(long unsigned));
    if (self_id == NULL){
        fprintf(stderr, "Invalid allocation of memmory in function 'mythread'.\n");
        return NULL;
    }
    *self_id = pthread_self();
	return (void*)self_id;
}

int main() {
	pthread_t tid[THREADS_COUNT];

    for(int th_num = 0; th_num < THREADS_COUNT; th_num++){
        int is_err = pthread_create(&tid[th_num], NULL, mythread, NULL);
        if (is_err != SUCCESS) {
            perror("bad work with pthread_create().\n");
            return ERROR;
        }
    }

    getchar();
    
	for(int th_num = 0; th_num < THREADS_COUNT; th_num++){
        void* thread_result;
        int is_err = pthread_join(tid[th_num], &thread_result);
        if (is_err != SUCCESS) {
            free((long*)thread_result);
            perror("bad work with pthread_join().\n");
            return ERROR;
        }
        if (thread_result == NULL){
            return ERROR;
        }
            
        long unsigned result_value = *((long unsigned*)thread_result);
        free((long*)thread_result);

        printf("thread num %d: ",th_num);
        if (result_value == tid[th_num])
            printf("ids are equal!!! %ld == %ld\n", tid[th_num], result_value);
        else
            printf("ids are not equal!!! %ld != %ld\n", tid[th_num], result_value);
    }
	
	return 0;
}

