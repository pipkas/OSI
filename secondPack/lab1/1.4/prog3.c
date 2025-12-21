#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#define TRUE 1
#define SUCCESS 0
#define ERROR -2
#define STRING_LENGTH 13
#define TO_CLEANUP 1

void cleanup(void *arg) {
    char *str = (char*)arg;
    printf("Freeing memory...\n");
    free(str);
}

void* mythread(void* arg){
    (void)arg;
    char *string = malloc(STRING_LENGTH * sizeof(char));
    if (string == NULL) {
        perror("bad try to allocate the memory.\n");
        return (void*)ERROR;
    }
    strcpy(string, "hello world");
    pthread_cleanup_push(cleanup, string);
    while(TRUE){
        printf("%s", string);
        pthread_testcancel();
    }
    pthread_cleanup_pop(TO_CLEANUP);
    
    return NULL;
}

int main(){
    pthread_t tid;
    int is_err = pthread_create(&tid, NULL, mythread, NULL);
	if (is_err != SUCCESS) {
		perror("bad work with pthread_create().\n");
		return ERROR;
	}
    getc(stdin);
    is_err = pthread_cancel(tid);
	if (is_err != SUCCESS) {
		perror("bad work with pthread_cancel().\n");
		return ERROR;
	}
    
    is_err = pthread_join(tid, NULL);
	if (is_err != SUCCESS) {
		perror("bad work with pthread_join().\n");
		return ERROR;
	}

    return 0;
}