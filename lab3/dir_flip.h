#pragma once

static const char MY_ERROR = 1;
static const int SYS_ERROR = -1;
static const unsigned int DIR_PERMISSION = 0755;
static const unsigned int FILE_PERMISSION = 0644;

void reverse_string(char *str, int str_len);
void make_flip_dir_path(char* cur_dir_path, char* flip_dir_path);
char deep_dir_flip(char* cur_dir_path_name, char* flip_dir_path_name);
char copy_file_reversed(char *src_path, char *dst_path);