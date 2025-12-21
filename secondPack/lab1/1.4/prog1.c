#include <stdio.h>
#include <pthread.h>
#include <string.h>
#define TRUE 1
#define SUCCESS 0
#define ERROR -1

void* mythread(void* arg){
    (void)arg;
    char string[] = "akula Matata";
    while(TRUE)
        printf("%s\n", string);
    
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