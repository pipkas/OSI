#include <stdio.h>
#include "file_processing.h"

int main(int argc, char* argv[]){
    char* prog_name = argv[0];
    if (argc < 2){
        fprintf(stderr, "Wrong number of arguments!");
        return ERROR;
    }
    operation_as_num op = find_operation(prog_name);
    int is_error = 0;
    switch(op){
        case MAKE_DIR:
            is_error = make_dir(argv[1]);   break;
        case MAKE_FILE:
            is_error = make_file(argv[1]);   break;
        case MAKE_SYM:
            if (argc == 3){
                is_error = make_sym_link(argv[1], argv[2]);  break;
            }
            is_error = make_sym_link(NULL, argv[1]);  break;
        case MAKE_HARD:
            if (argc == 3){
                is_error = make_hard_link(argv[1], argv[2]);  break;
            }
            is_error = make_hard_link(NULL, argv[1]);  break;
        case RM_DIR:
            is_error = rm_dir(argv[1]);     break;
        case RM_FILE:
            is_error = rm_file(argv[1]);    break;
        case RM_SYM:
            is_error = rm_sym_link(argv[1]);    break;
        case RM_HARD:
            is_error = rm_hard_link(argv[1]);   break;
        case PRINT_FILE:
            is_error = print_file(argv[1]); break;
        case PRINT_DIR:
            is_error = print_dir(argv[1]);  break;
        case PRINT_SYM:
            is_error = print_sym_link(argv[1]);  break;
        case PRINT_PERM:
            is_error = print_perm(argv[1]); break;
        case PRINT_FILE_USING_SYM:
            is_error = print_file_using_sym_link(argv[1]);   break;
        case CHANGE_PERM:
            if (argc == 3){
                is_error = change_perm(argv[1], argv[2]);  break;
            }
            is_error = change_perm(argv[1], NULL);  break;
        default:
            is_error = ERROR; break;
    }
    if (is_error == ERROR)
        return ERROR;
}