#include <string.h>
#include <stdio.h>
#include "file_processing.h"
#define NUM_OF_OPERATIONS 14

operation_as_num find_operation(const char* op_name){
    op_name = strrchr(op_name, '/') + 1;

    operation group_of_operations[NUM_OF_OPERATIONS] = {
        {"print_dir", PRINT_DIR}, 
        {"print_file", PRINT_FILE},
        {"print_sym_link", PRINT_SYM}, 
        {"print_file_using_sym_link", PRINT_FILE_USING_SYM},
        {"print_perm_and_num_of_hard_links", PRINT_PERM}, 
        {"rm_dir", RM_DIR}, 
        {"rm_file", RM_FILE},
        {"rm_sym_link", RM_SYM}, 
        {"rm_hard_link", RM_HARD}, 
        {"change_perm", CHANGE_PERM}, 
        {"make_dir", MAKE_DIR}, 
        {"make_file", MAKE_FILE},
        {"make_sym_link", MAKE_SYM}, 
        {"make_hard_link", MAKE_HARD}
    };

    for (int i = 0; i < NUM_OF_OPERATIONS; i++){
        int is_equals = strcmp(op_name, group_of_operations[i].op_name);
        if (is_equals == 0)
            return group_of_operations[i].op_num;
    }
    fprintf(stderr, "There is not %s operation!\n", op_name);
    return NO_OP;
}
