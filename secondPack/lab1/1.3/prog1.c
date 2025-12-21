#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#define SUCCESS 0
#define ERROR -1
#define OBJ_VAL 12
#define OBJ_STR "13asdfghjkl;'dsfghjkl;'dfgyuhijtfyguhjkl"

typedef struct Object{
    int num;
    char* str;
}Object;

void *mythread(void *arg) {
	Object* func_arg = (Object*)arg;
    if (func_arg == NULL){
        printf("Not correct argument in 'mythread' function\n");
        return NULL;
    }
	printf("object values: %d, %s\n", func_arg->num, func_arg->str);
	return NULL;
}

int main() {
	pthread_t tid;
    Object obj = {OBJ_VAL, OBJ_STR};

	int is_err = pthread_create(&tid, NULL, mythread, (void*)(&obj));
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

