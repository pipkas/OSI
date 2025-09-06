#include <stdio.h>
#include <unistd.h>
#include "get_address.h"

typedef enum points{
    A = 'a',
    D = 'd',
    E = 'e',
    I = 'i'
} points;

int main(int argc, char* argv[]){
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
        case A:{
            print_vars_adr();
            sleep(100);
            break;
        }
        case D:
            printf("The address of local variable is %p\n", return_local_var());    break;
        case E:
            is_error = experiment_with_malloc();    break;
        case I:
            is_error = print_and_change_env_var();  break;
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