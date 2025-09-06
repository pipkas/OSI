#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include "wait_and_check.h"

int wait_and_check(){
    int status;
    pid_t child_pid = wait(&status); 

    if (child_pid == ERROR) {
        perror("Error in function 'wait'\n");
        return ERROR;
    }

    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        printf("Child process (PID: %d) exited with code %d\n", child_pid, exit_code);
        return 0;
    } 
    
    if (WIFSIGNALED(status)) {
        int signal_number = WTERMSIG(status);
        printf("Child process (PID: %d) was killed %d (%s)\n", child_pid, signal_number, strsignal(signal_number));
        return ERROR;
    }
    
    printf("Child process (PID: %d) exited with unknown reason\n", child_pid);
    return ERROR;
}