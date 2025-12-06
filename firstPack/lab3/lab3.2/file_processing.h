#pragma once

static const int ERROR = -1;

typedef enum {
    PRINT_FILE = 0,
    PRINT_DIR = 1,
    PRINT_SYM = 2,
    PRINT_PERM = 3,
    PRINT_FILE_USING_SYM = 4,
    MAKE_DIR = 5,
    MAKE_FILE = 6,
    MAKE_SYM = 7,
    MAKE_HARD = 8,
    RM_DIR = 9,
    RM_FILE = 10,
    RM_SYM = 11,
    RM_HARD = 12,
    CHANGE_PERM = 13,
    NO_OP = 14
} operation_as_num;

typedef struct{
    char* op_name;
    operation_as_num op_num;
} operation;

int rm_dir(char* dir_path_name);
int rm_file(char* file_path_name);
int rm_sym_link(char* file_path_name);
int rm_hard_link(char* file_path_name);
int make_hard_link(char* link_name, char* file_path_name);
int make_sym_link(char* link_name, char* file_path_name);
int make_file(char* file_path_name);
int make_dir(char* dir_path_name);
int print_dir(char* dir_path_name);
int print_file(char* file_path_name);
int print_sym_link(char* link_path_name);
int print_file_using_sym_link(char* link_path_name);
int print_perm(char* file_path_name);
int change_perm(char* file_path_name, char* perm);

operation_as_num find_operation(const char* op_name);
