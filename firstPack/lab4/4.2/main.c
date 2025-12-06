#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include "addr_space_manage.h"

typedef enum points{
    A = 'a',
    C = 'c'
} points;


int main(int argc, char* argv[], char* envp[]){

    if (argc != 2){
        fprintf(stderr, "Wrong number of arguments.\n");
        return ERROR;
    }
    if (argv[1][1] != '\0'){//введен не один символ
        fprintf(stderr, "Incorrect argument.\n");
        return ERROR;
    }
    int is_error = NO_ERROR;

    points point = argv[1][0];
    switch(point){
        case A:
            is_error = test_exec(argv, envp);   break;
        case C:
            is_error = experiment();    break;
        default:{
            fprintf(stderr, "There is not such point.\n");    
            is_error = ERROR;
        }
    }
    if (is_error == ERROR){
        return ERROR;
    }
    return 0;
}

