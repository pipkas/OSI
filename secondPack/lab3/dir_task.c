#include "task.h"

int make_path(char* path, size_t size, const char* dir, const char* name) {
    int len_path = snprintf(path, size, "%s/%s", dir, name);
    if (len_path < 0 || (size_t)len_path >= size) {
        return ERROR;
    }
    return SUCCESS;
}


int process_entry(const char* src_dir, const char* dst_dir, const char* entry_name) {
    int err;
    char src_path[PATH_MAX];
    char dst_path[PATH_MAX];
    struct stat stat_buf;
    err = make_path(src_path, sizeof(src_path), src_dir, entry_name);
    if (err != SUCCESS) {
        printf("process_entry: source path too long: %s/%s\n", src_dir, entry_name);
        return ERROR;
    }    
    err = make_path(dst_path, sizeof(dst_path), dst_dir, entry_name);
    if (err != SUCCESS) {
        printf("process_entry: destination path too long: %s/%s\n", dst_dir, entry_name);
        return ERROR;
    }

    err = lstat(src_path, &stat_buf);
    if (err != SUCCESS) {
        printf("process_entry: lstat() failed for %s: %s\n", src_path, strerror(errno));
        return ERROR;
    }
    if (S_ISDIR(stat_buf.st_mode)) {
        return create_dir_task(src_path, dst_path);
    }
    if (S_ISREG(stat_buf.st_mode)) {
        return create_file_task(src_path, dst_path);
    }    
    return SUCCESS;
}


//крутимся в цикле когда слишком много открытых файлов
DIR* opendir_with_retry(const char* path) {
    DIR* dir;
    int retries = 0;    
    while (retries < MAX_RETRIES) {
        dir = opendir(path);
        if (dir != NULL) {
            return dir; 
        }         
        if (errno != EMFILE) {
            printf("opendir_with_retry: opendir() failed for %s: %s\n", path, strerror(errno));
            return NULL;
        }     
        retries++;
        sleep(1);
    }
    return NULL;
}



int create_dir(const char* src_path, const char* dst_path) {
    int err; 
    struct stat src_stat;
    err = lstat(src_path, &src_stat);
    if (err != SUCCESS) {
        printf("create_dir: lstat() failed for %s: %s\n", src_path, strerror(errno));
        return ERROR;
    }

    err = mkdir(dst_path, src_stat.st_mode);
    if (err != SUCCESS && errno != EEXIST) {
        printf("create_dir: mkdir() failed for %s: %s\n", src_path, strerror(errno));
        return ERROR;
    } 
    return SUCCESS;
}



void *work_dir_thread(void* arg) {
    int err;
    task_t* task = (task_t*)arg;
    err = create_dir(task->src_path, task->dst_path);
    if (err != SUCCESS) {
        printf("work_dir_thread: failed to create directory %s\n", task->dst_path);
        free(task);
        return NULL;
    }    

    DIR* dir = opendir_with_retry(task->src_path);
    if (dir == NULL) {
        printf("work_dir_thread: failed to open directory %s\n", task->src_path);
        free(task);
        return NULL;
    }

    struct dirent* entry;
    while (1) {
        errno = SUCCESS;
        entry = readdir(dir); 
        if (entry == NULL && errno != SUCCESS) {
            printf("work_dir_thread: readdir error: %s\n", strerror(errno));
            err = ERROR;
            break;
        }   
        if (entry == NULL) {
            err = SUCCESS;
            break; 
        }                
        if (strcmp(entry->d_name, CUR_DIR) == 0 || strcmp(entry->d_name, PARENT_DIR) == 0) {
            continue;
        }
        err = process_entry(task->src_path, task->dst_path, entry->d_name);
        if (err != SUCCESS) {
            printf("work_dir_thread: failed to add task for %s\n", entry->d_name);
            break;
        }
    } 
    err = closedir(dir);
    if (err != SUCCESS) {
        printf("work_dir_thread: closedir() failed: %s\n", strerror(errno));
    }    
    free(task);
    return NULL;
}



int create_dir_task(const char* src_path, const char* dst_path) {
    int err;
    pthread_t thread;
    task_t* task = malloc(sizeof(task_t));
    if (task == NULL) {
        printf("create_dir_task: memory allocation failed\n");
        return ERROR;
    }  
    strcpy(task->src_path, src_path);
    strcpy(task->dst_path, dst_path);        
    
    int retries = 0;
    err = pthread_create(&thread, NULL, work_dir_thread, task);
    while (err == EAGAIN && retries < MAX_RETRIES){
        sleep(1);
        retries++;
        err = pthread_create(&thread, NULL, work_dir_thread, task);
    }
	if (err != SUCCESS) {
		printf("create_dir_task: pthread_create() failed: %s\n", strerror(err));
        free(task);
		return ERROR;
	}
    err = pthread_detach(thread);
    if (err != SUCCESS) {
        printf("create_dir_task: pthread_detach() failed: %s\n", strerror(err));
    }    
    return SUCCESS;
}

