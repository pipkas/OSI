#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "wait_and_check.h"

int global_var = 3;
int const CHILD_PROC = 0;

int main(){
    int local_var = 7;
    printf("address of local var: %p, value: %d\n", (void*)&local_var, local_var);
    printf("address of global var: %p, value: %d\n", (void*)&global_var, global_var);
    printf("pid: %d\n", getpid());
    
    int process = fork();
    if (process == ERROR){
        perror("Error in function 'fork'\n");
        return ERROR;
    }

    if (process == CHILD_PROC){
        printf("child process: pid: %d\n", getpid());
        printf("child process: parent pid: %d\n", getppid());
        printf("child process: address of local var: %p, value: %d\n", (void*)&local_var, local_var);
        printf("child process: address of global var: %p, value: %d\n",(void*)&global_var, global_var);
        local_var = 33;
        global_var = 77;
        printf("child process: new value of local var = %d , new value of global var = %d\n", local_var, global_var);
        _exit(5);
    }
    
    getc(stdin);
    printf("parent process: value of local var = %d, value of global var = %d\n", local_var, global_var);
    int is_error = wait_and_check();
    if (is_error == ERROR)
        return ERROR;
    
    return 0;
}