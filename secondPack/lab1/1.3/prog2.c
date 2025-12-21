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

void free_object(Object* obj){
    free(obj->str);
    free(obj);
}

void *mythread(void *arg) {
	Object* func_arg = (Object*)arg;
    if (func_arg == NULL){
        printf("Not correct argument in 'mythread' function\n");
        return NULL;
    }
	printf("object values: %d, %s\n", func_arg->num, func_arg->str);
    free_object(func_arg);
	return NULL;
}

int main() {
    int is_err;
	pthread_t tid;
    pthread_attr_t attr;
    is_err = pthread_attr_init(&attr);
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
    Object *obj = malloc(sizeof(Object));
    if (obj == NULL){
        perror("bad try to allocate the memory.\n");
		return ERROR;
    }
    obj->num = OBJ_VAL;
    obj->str = strdup(OBJ_STR);
    if (obj->str == NULL){
        free(obj);
        pthread_attr_destroy(&attr);
        perror("Error with 'strdup' in 'main' function\n");
        return ERROR;
    }
	is_err = pthread_create(&tid, &attr, mythread, (void*)obj);
	if (is_err != SUCCESS) {
        free_object(obj);
        pthread_attr_destroy(&attr);
		perror("bad work with pthread_create().\n");
		return ERROR;
	}
    pthread_attr_destroy(&attr);
    pthread_exit(SUCCESS);
}

