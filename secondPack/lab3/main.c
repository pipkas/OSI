#include "task.h"

pthread_mutex_t dir_mutex;

int main(int argc, char* argv[]) {   
    int err;
    struct stat stat_buf; 
    pthread_t main_thread;
    if (argc != 3) {
        printf("Use %s source_directory target_directory\n", argv[0]);
        return ERROR;
    }        
    err = lstat(argv[1], &stat_buf);
    if (err != SUCCESS) {
        printf("main: lstat() failed: %s\n", strerror(errno));
        return ERROR;
    }
    if (S_ISDIR(stat_buf.st_mode) != true) {
        printf("main: Source path %s is not a directory\n", argv[1]);
        return ERROR;
    }

    err = pthread_mutex_init(&dir_mutex, NULL);
	if (err != SUCCESS) {
		printf("main: pthread_mutex_init() failed: %s\n", strerror(err));
		return ERROR;
	}

    task_t* task = malloc(sizeof(task_t));
    if (task == NULL) {
        printf("main: memory allocation failed\n");
        err = pthread_mutex_destroy(&dir_mutex);
        if (err != SUCCESS) {
            printf("main: pthread_mutex_destroy() failed: %s\n", strerror(err));
        }
        return ERROR;
    }
    strcpy(task->src_path, argv[1]);
    strcpy(task->dst_path, argv[2]);
    
    err = pthread_create(&main_thread, NULL, work_directory_thread, task);
	if (err != SUCCESS) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
        free(task);
		err = pthread_mutex_destroy(&dir_mutex);
		if (err != SUCCESS) {
            printf("main: pthread_mutex_destroy() failed: %s\n", strerror(err));
        }
		return ERROR;
	}
    err = pthread_join(main_thread, NULL);
	if (err != SUCCESS) {
		printf("main: pthread_join() failed: %s\n", strerror(err));
        err = pthread_mutex_destroy(&dir_mutex);
		if (err != SUCCESS) {
            printf("main: pthread_mutex_destroy() failed: %s\n", strerror(err));
        }
		return ERROR;
	}
    sleep(2);
    err = pthread_mutex_destroy(&dir_mutex);
	if (err != SUCCESS) {
		printf("main: pthread_mutex_destroy() failed: %s\n", strerror(err));
	}
    return SUCCESS;
}