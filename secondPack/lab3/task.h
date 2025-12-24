#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <pthread.h>
#include <limits.h>

#define SUCCESS 0
#define ERROR -1
#define BUFFER_SIZE 8192
#define MAX_RETRIES 10
#define NULL_TERM_SIZE 1

typedef struct {
    char src_path[PATH_MAX];
    char dst_path[PATH_MAX];
} task_t;

extern pthread_mutex_t dir_mutex;

int create_file_task(const char* src_path, const char* dst_path);
int create_dir_task(const char* src_path, const char* dst_path);
void *work_dir_thread(void* arg);
