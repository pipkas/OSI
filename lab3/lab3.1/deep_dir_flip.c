#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include "dir_flip.h"

const char* PARENT_DIR = "..";
const char* CUR_DIR = ".";

void make_flip_dir_path(char* cur_dir_path, char* flip_dir_path)
{
    strcpy(flip_dir_path, cur_dir_path);
    char* helper_ptr = strrchr(flip_dir_path, '/');
    char* base_name = (helper_ptr == NULL) ? flip_dir_path : (helper_ptr + 1);
    reverse_string(base_name, strlen(base_name));
}


static char flip_element(struct dirent *entry, char* cur_dir_path_name, char* flip_dir_path_name)
{
    int len_name = strlen(entry->d_name);
    char src_path[strlen(cur_dir_path_name) + len_name + 2];
    snprintf(src_path, sizeof(src_path), "%s/%s", cur_dir_path_name, entry->d_name);

    struct stat st;
    int is_error = lstat(src_path, &st);
    if (is_error == SYS_ERROR) {
        perror("Error getting info about file ");
        fprintf(stderr, "%s\n", src_path);
        return MY_ERROR;
    }

    if (!S_ISDIR(st.st_mode) && !S_ISREG(st.st_mode))
        return 0;
    
    char reversed_name[len_name + 1];
    strcpy(reversed_name, entry->d_name);
    reverse_string(reversed_name, len_name);
    
    char dest_path [strlen(flip_dir_path_name) + len_name + 2];
    snprintf(dest_path, sizeof(dest_path), "%s/%s", flip_dir_path_name, reversed_name);

    if (S_ISDIR(st.st_mode))
        is_error = deep_dir_flip(src_path, dest_path);
        
    if (S_ISREG(st.st_mode))
        is_error = copy_file_reversed(src_path, dest_path); 

    if (is_error == MY_ERROR)
        return MY_ERROR;
    return 0;
}



char deep_dir_flip(char* cur_dir_path_name, char* flip_dir_path_name)
{

    DIR *open_dir = opendir(cur_dir_path_name);
    if (open_dir == NULL) {
        perror("Error opening directory ");
        fprintf(stderr, "%s\n", cur_dir_path_name);
        return MY_ERROR;
    }

    int is_error = mkdir(flip_dir_path_name, DIR_PERMISSION); 
    if (is_error != 0 && errno != EEXIST) {
        closedir(open_dir);
        perror("Error creating reversed directory ");
        fprintf(stderr, "%s\n", flip_dir_path_name);
        return MY_ERROR;
    }

    struct dirent *entry;
    
    while ((entry = readdir(open_dir)) != NULL) {
        if (strcmp(entry->d_name, CUR_DIR) == 0 || strcmp(entry->d_name, PARENT_DIR) == 0) {
            continue;
        }
        is_error = flip_element(entry, cur_dir_path_name, flip_dir_path_name);    
        if (is_error != 0){
            closedir(open_dir);
            return MY_ERROR;  
        }
    }
    
    closedir(open_dir);
    return 0;
}

