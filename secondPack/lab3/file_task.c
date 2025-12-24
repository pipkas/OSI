#include "task.h"

int open_with_retry(const char* path, int flags, mode_t mode) {
    int fd;
    int retries = 0;
    while (retries < MAX_RETRIES) {
        fd = open(path, flags, mode);
        if (fd != ERROR) {
            return fd;  
        }      
        if (errno != EMFILE) {
            printf("open_with_retry: open() failed for %s: %s\n", path, strerror(errno));
            return ERROR;
        }     
        retries++;
        sleep(1);
    }
    return ERROR;
}


void *copy_file_thread(void* arg) {
    int err;
    task_t* task = (task_t*)arg;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;
    struct stat src_stat;
    err = lstat(task->src_path, &src_stat);
    if (err != SUCCESS) {
        printf("copy_file_thread: lstat() failed for %s: %s\n", task->src_path, strerror(errno));
        free(task);
        return NULL;
    }

    int src_fd = open_with_retry(task->src_path, O_RDONLY, 0);
    if (src_fd == ERROR) {
        printf("copy_file_thread: failed to open source %s\n", task->src_path);
        free(task);
        return NULL;
    }    
    int dst_fd = open_with_retry(task->dst_path, O_WRONLY | O_CREAT | O_TRUNC, src_stat.st_mode);
    if (dst_fd == ERROR) {
        printf("copy_file_thread: failed to create target %s\n", task->dst_path);
        err = close(src_fd);
        if (err != SUCCESS) {
            printf("copy_file_thread: close() failed for source fd: %s\n", strerror(errno));
        }
        free(task);
        return NULL;
    }
    
    while (1) {
        bytes_read = read(src_fd, buffer, BUFFER_SIZE);
        if (bytes_read == ERROR) {
            printf("copy_file_thread: read() error from %s: %s\n", task->src_path, strerror(errno));
            break;
        }        
        if (bytes_read == 0) {
            break;  
        }
        
        int write_error = 0;
        ssize_t total_written = 0;
        char* ptr = buffer;
        while (total_written < bytes_read) {
            bytes_written = write(dst_fd, ptr + total_written, bytes_read - total_written);
            if (bytes_written == ERROR) {
                printf("copy_file_thread: write error to %s: %s\n", task->dst_path, strerror(errno));
                write_error = 1;
                break;
            }
            total_written += bytes_written;
        }
        if (write_error) {
            break;  
        }        
    }
    err = close(src_fd);
    if (err != SUCCESS) {
        printf("copy_file_thread: close() failed for source fd: %s\n", strerror(errno));
    }    
    err = close(dst_fd);
    if (err != SUCCESS) {
        printf("copy_file_thread: close() failed for target fd: %s\n", strerror(errno));
    }    
    free(task);
    return NULL;
}


int create_file_task(const char* src_path, const char* dst_path) {
    int err;
    pthread_t thread;
    task_t* task = malloc(sizeof(task_t));
    if (task == NULL) {
        printf("create_file_task: memory allocation failed\n");
        return ERROR;
    }
    strcpy(task->src_path, src_path);
    strcpy(task->dst_path, dst_path);    
    
    err = pthread_create(&thread, NULL, copy_file_thread, task);
    while (err == EAGAIN){
        sleep(1);
        err = pthread_create(&thread, NULL, copy_file_thread, task);
    }
	if (err != SUCCESS) {
		printf("create_file_task: pthread_create() failed: %s\n", strerror(err));
        free(task);
		return ERROR;
	}
    err = pthread_detach(thread);
    if (err != SUCCESS) {
        printf("create_file_task: pthread_detach() failed: %s\n", strerror(err));
    }    
    return SUCCESS;
}



