#include "get_address.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int global_init_var = 5;
static int global_uninit_var;
static const int global_const = 5;


void print_vars_adr(){
    int local_var;
    static int static_var;
    const int const_var = 5;
    printf("\nThe address of local variable is %p\n", (void*)&local_var);
    printf("The address of local const variable is %p\n", (void*)&const_var);
    printf("The address of static variable is %p\n", (void*)&static_var);
    printf("The address of global initialized variable is %p\n", (void*)&global_init_var);
    printf("The address of global uninitialized variable is %p\n", (void*)&global_uninit_var);
    printf("The address of global const variable is %p\n", (void*)&global_const);
}

void* return_local_var(){
    int local_var = 5;
    return (void*)&local_var;
}

static char* new_massive(int* is_error){
    char* massive = (char*)malloc(100);
    if (massive == NULL){
        fprintf(stderr, "Error allocating memmory for massive in function 'experiment_with_malloc'.\n");
        perror("Error");
        *is_error = ERROR;
        return NULL;
    }
    strcpy(massive, "hello_world");
    return massive;
}

int experiment_with_malloc(){
    int is_error;
    char* massive = new_massive(&is_error);
    char* massive2 = new_massive(&is_error);
    if (is_error == ERROR)
        return ERROR;
    printf("%s\n", massive);
    free(massive);
    printf("%s\n", massive);
    
    printf("%s\n", massive2);
    char* middle_ptr = massive2 + 50;

    free(middle_ptr);
    printf("%s", massive2);
    return NO_ERROR;
}


int print_and_change_env_var() {
    const char* env_var_name = "MY_ENV_VAR";

    char* env_value = getenv(env_var_name);
    printf("The value of env var '%s' is '%s'\n", env_var_name, env_value ? env_value : "(not specified)");
 
    int is_error = setenv(env_var_name, "Hello, World!", 1);
    if (is_error != NO_ERROR) {
        perror("Error in func setenv.\n");
        return ERROR;
    }

    env_value = getenv(env_var_name);
    printf("New value of env var '%s' is '%s'\n", env_var_name, env_value);

    return 0;
}
