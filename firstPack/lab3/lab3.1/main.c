#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include "dir_flip.h"

int main(int argc, char *argv[]) 
{
    if (argc != 2) {
        fprintf(stderr, "Wrong number of input arguments\n");
        return MY_ERROR;
    }

    char* dir_path = argv[1];

    struct stat source_stat;
    char is_error = stat(dir_path, &source_stat);
    if (is_error == SYS_ERROR) {
        perror("Error checking source directory\n");
        return MY_ERROR;
    }

    if (!S_ISDIR(source_stat.st_mode)) {
        fprintf(stderr, "Error: input argument is not a directory\n");
        return MY_ERROR;
    }

    char flip_dir_path[strlen(dir_path) + 1];
    make_flip_dir_path(dir_path, flip_dir_path);

    is_error = deep_dir_flip(dir_path, flip_dir_path);
    if (is_error == MY_ERROR)
        return MY_ERROR;
    return 0;
}