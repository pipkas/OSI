#define _XOPEN_SOURCE 700
#include "file_processing.h"
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#define BUF_SIZE 1024
#define O_PATH 010000000

const unsigned int FILE_PERMISSION = 0644;
const unsigned int MAX_PERMISSION = 07777;
const unsigned int DEFAULT_PERMISSION = 0755;



static int get_mode(char* path){
    struct stat st;
    int is_error = lstat(path, &st);
    if (is_error == ERROR) {
        fprintf(stderr, "Error getting info about file %s.\n", path);
        perror("Error");
        return ERROR;
    }
    return st.st_mode;
}

int make_dir(char* dir_path_name){
    int is_error = mkdir(dir_path_name, DEFAULT_PERMISSION); 
    if (is_error == ERROR) {
        fprintf(stderr, "Error creating directory %s.\n", dir_path_name);
        perror("Error");
        return ERROR;
    }
    return 0;
}

int make_file(char* file_path_name){
    int file = open(file_path_name, O_CREAT, FILE_PERMISSION);
    if (file == ERROR) {
        fprintf(stderr, "Error creating file %s.\n", file_path_name);
        perror("Error");
        return ERROR;
    }
    close(file);
    return 0;
}

int make_sym_link(char* link_name, char* file_path_name){//проверить что файл подается???
    if (link_name == NULL)
        link_name = "new_sym_link";
    int is_error = symlink(file_path_name, link_name);
    if (is_error == ERROR){
        fprintf(stderr, "Error creating sym link to a file %s.\n", file_path_name);
        perror("Error");
        return ERROR;
    }
    return 0;
}

int make_hard_link(char* link_name, char* file_path_name){//проверить что файл подается???
    if (link_name == NULL)
        link_name = "new_hard_link";
    int is_error = link(file_path_name, link_name);
    if (is_error == ERROR){
        fprintf(stderr, "Error creating hard link to a file %s.\n", file_path_name);
        perror("Error");
        return ERROR;
    }
    return 0;
}


int rm_file(char* file_path_name){

    int mode = get_mode(file_path_name);
    if (mode == ERROR)
        return ERROR;
    if (!S_ISREG(mode)){
        fprintf(stderr, "Error removing file %s, because it is not regular file.\n", file_path_name);
        return ERROR;
    }
    int is_error = remove(file_path_name);
    if (is_error == ERROR) {
        fprintf(stderr, "Error removing file %s.\n", file_path_name);
        perror("Error");
        return ERROR;
    }
    return 0;
}

int rm_sym_link(char* file_path_name){

    int mode = get_mode(file_path_name);
    if (mode == ERROR)
        return ERROR;
    if (!S_ISLNK(mode)){
        fprintf(stderr, "Error removing file %s, because it is not sym link.\n", file_path_name);
        return ERROR;
    }
    int is_error = remove(file_path_name);
    if (is_error == ERROR) {
        fprintf(stderr, "Error removing sym link %s.\n", file_path_name);
        perror("Error");
        return ERROR;
    }
    return 0;
}

int rm_hard_link(char* file_path_name){

    int mode = get_mode(file_path_name);
    if (mode == ERROR)
        return ERROR;
    if (!S_ISLNK(mode) && !S_ISREG(mode)){
        fprintf(stderr, "Error removing file %s, because it is not hard link.\n", file_path_name);
        return ERROR;
    }
    int is_error = remove(file_path_name);
    if (is_error == ERROR) {
        fprintf(stderr, "Error removing hard link %s.\n", file_path_name);
        perror("Error");
        return ERROR;
    }
    return 0;
}

static int rm_element_in_dir(const char *path, const struct stat *sb, int type, struct FTW *ftwbuf){
    (void)sb; (void)type; (void)ftwbuf;

    int is_error = remove(path);
    if (is_error == ERROR) {
        fprintf(stderr, "Error removing file %s.\n", path);
        perror("Error");
        return ERROR; 
    }
    return 0;
}

int rm_dir(char* dir_path_name){
    int mode = get_mode(dir_path_name);
    if (mode == ERROR)
        return ERROR;
    if (!S_ISDIR(mode)){
        fprintf(stderr, "Error removing dir, because %s is not dir.\n", dir_path_name);
        return ERROR;
    }
    int is_error = nftw(dir_path_name, rm_element_in_dir, 64, FTW_DEPTH | FTW_PHYS);
    if (is_error == ERROR){
        fprintf(stderr, "Error removing dir %s.\n", dir_path_name);
        perror("Error");
    }
    return 0;
}

int print_dir(char* dir_path_name){

    struct dirent **name_list;
    int count_in_dir = scandir(dir_path_name, &name_list, NULL, NULL);
    if (count_in_dir < 0) {
        fprintf(stderr, "Error printing dir %s.\n", dir_path_name);
        perror("Error in scandir func");
        return ERROR;
    }
    for (int entry = 0; entry < count_in_dir; entry++) {
        printf("%s\n", name_list[entry]->d_name);
        free(name_list[entry]);
    }
    free(name_list);
    return 0;
}

static int print_src(char* src_path_name, unsigned int src_perm){
    int file = open(src_path_name,  src_perm);
    if (file == ERROR) {
        fprintf(stderr, "Error opening file %s.\n", src_path_name);
        return ERROR;
    }
    char buf[BUF_SIZE];
    ssize_t bytes_read = read(file, buf, sizeof(buf));

    while (bytes_read > 0) {
        if (write(STDOUT_FILENO, buf, bytes_read) != bytes_read) {
            close(file);
            fprintf(stderr, "Error opening file %s.\n", src_path_name);
            return ERROR;
        }
        bytes_read = read(file, buf, sizeof(buf));
    }
    if (bytes_read == ERROR){
        close(file);
        fprintf(stderr, "Error opening file %s.\n", src_path_name);
        return ERROR;
    }
    return 0;
}

int print_file(char* file_path_name){
    int mode = get_mode(file_path_name);
    if (mode == ERROR)
        return ERROR;
    if (!S_ISREG(mode)){
        fprintf(stderr, "Error printing file %s, because it is not regular file.\n", file_path_name);
        return ERROR;
    }
    int is_error = print_src(file_path_name, O_RDONLY);

    if (is_error == ERROR){
        perror("Error printing file content");
        return ERROR;
    }
    return 0;
}

int print_sym_link(char* link_path_name){
    int mode = get_mode(link_path_name);
    if (mode == ERROR)
        return ERROR;
    if (!S_ISLNK(mode)){
        fprintf(stderr, "Error printing link content %s, because it is not sym link.\n", link_path_name);
        return ERROR;
    }

    char link_content[BUF_SIZE];
    ssize_t len = readlink(link_path_name, link_content, sizeof(link_content) - 1);
    if (len == ERROR) {
        perror("Error printing sym link content because of readlink func\n");
        return ERROR;
    }

    link_content[len] = '\0';
    printf("Symlink '%s' points to: '%s'\n", link_path_name, link_content);
    return 0;
}

int print_file_using_sym_link(char* link_path_name){
    int mode = get_mode(link_path_name);
    if (mode == ERROR)
        return ERROR;
    if (!S_ISLNK(mode)){
        fprintf(stderr, "Error printing file using sym link %s, because it is not sym link.\n",link_path_name);
        return ERROR;
    }

    int is_error = print_src(link_path_name, O_RDONLY);

    if (is_error == ERROR){
        perror("Error printing file content");
        return ERROR;
    }
    return 0;
}

int change_perm(char* file_path_name, char* perm){
    unsigned int mode = 0755; 
    if (perm != NULL){
        char* end_ptr;
        mode = strtoul(perm, &end_ptr, 8);
        if (*end_ptr != '\0'){
            fprintf(stderr, "Error setting mode %s on file %s because %c is incorrect symbol\n",  \
                    perm, file_path_name, *end_ptr);
            return ERROR;
        }
        if (errno == ERANGE || mode > MAX_PERMISSION) {
            fprintf(stderr, "Error: mode '%s' is too large (max 07777). Error while changing permissions\n", perm);
            return ERROR;
        }
    }
    int is_error = chmod(file_path_name, mode);
    if (is_error == ERROR){
        fprintf(stderr, "Error setting mode %s on file %s\n", perm, file_path_name);
        perror("Error setting mode on file %s\n");
        return ERROR;
    }
    return 0;
}

int print_perm(char* file_path_name){
    struct stat st;
    int is_error = lstat(file_path_name, &st);
    if (is_error == ERROR) {
        fprintf(stderr, "Error getting info about file %s.\n", file_path_name);
        perror("Error");
        return ERROR;
    }
    printf("Mode of file %s : %04o\n", file_path_name, st.st_mode & 07777);
    printf("Number of hard links on file %s: %ld\n", file_path_name, st.st_nlink);
    return 0;
}