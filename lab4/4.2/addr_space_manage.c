#define _GNU_SOURCE
#include "addr_space_manage.h"
#include <sys/types.h>
#include <stdio.h>
#include <poll.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

const int SIZEOF_PTR_ARRAY = 300;
const int NUM_OF_PAGES = 10;
const int START_PAGE_TO_MUNMAP = 4;
const int END_PAGE_TO_MUNMAP = 6;
const int NUM_OF_PAGES_TO_MUNMAP = END_PAGE_TO_MUNMAP - START_PAGE_TO_MUNMAP;

static sigjmp_buf jump_buffer;

static void catch_enter() {
    while (getchar() != '\n');
    getchar();
}

int test_exec(char* argv[], char* envp[]){
    pid_t pid = getpid();
    printf("PID: %d\n", pid);

    sleep(1);

    int is_error = execve("/proc/self/exe", argv, envp);

    if (is_error == ERROR) {
        perror("execve failed");
        return ERROR;
    }

    printf("Hello world\n");
    return NO_ERROR;
}

static int check_input_and_wait_a_sec(){
    struct timeval start_time, current_time;
    long elapsed_time = 0;
    gettimeofday(&start_time, NULL);

    struct pollfd fds[1];
    fds[0].fd = STDIN_FILENO; 
    fds[0].events = POLLIN; 
    
    while (elapsed_time <= SEC) {
        int ret = poll(fds, 1, 0);
        if (ret == ERROR){
            perror("Error in function 'poll'.\n");
            return ERROR;
        }
        if (fds[0].revents & POLLIN)
            return END;

        gettimeofday(&current_time, NULL);
        elapsed_time = (current_time.tv_sec - start_time.tv_sec) * SEC + (current_time.tv_usec - start_time.tv_usec);
    }
    return NO_ERROR;
}

static int array_alloc_on_stack(){
    char massive[_4KB];
    (void)massive;
    printf("A new array has been created on the stack.\n");
    
    int return_val = check_input_and_wait_a_sec();
    switch (return_val){
        case ERROR: return ERROR;
        case END:   return NO_ERROR;
    }
    
    int is_error = array_alloc_on_stack();
    return is_error;
}

static int array_alloc_in_heap(){
    char* ptr_array[SIZEOF_PTR_ARRAY];
    int num_of_iter = 0;
    int is_end = NO_END;
    int is_error = NO_ERROR;

    while (is_end == NO_END && is_error == NO_ERROR){
        ptr_array[num_of_iter] = (char*)malloc(_4KB);
        if (ptr_array[num_of_iter] == NULL){
            perror("Error allocating memmory for massive in function 'array_alloc_in_heap'.\n");
            is_error = ERROR;   
            break;
        }
        printf("A new array has been created in the heap.\n");
        num_of_iter++;

        int return_val = check_input_and_wait_a_sec();
        switch (return_val){
            case ERROR: is_error = ERROR;    break;
            case END:   is_end = END;   break;
        }
        if (num_of_iter + 1 >= SIZEOF_PTR_ARRAY){
            printf("Memory has been allocated for five minutes, the array limit has been reached.");
        }
    }
    printf("Press 'Enter' if you want to free memory.\n");
    catch_enter();

    for (int i = 0; i < num_of_iter; i++){
        free(ptr_array[i]);
    }
    printf("Memory released.\n");
    return is_error;
}

void segfault_handler(int sig) {
    printf("Signal SIGSEGV intercepted (Segmentation Fault). Signal code: %d\n", sig);
    siglongjmp(jump_buffer, 1);
}

int anonym_mem_game() {
    struct sigaction sa = {
        .sa_handler = segfault_handler,
        .sa_flags = SA_NODEFER,  // Не блокировать SIGSEGV во время обработки
    };
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("Error with sigaction in anonym_mem_game function.\n");
        return ERROR;
    }
    
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size == ERROR) {
        perror("Error with sysconf in anonym_mem_game function.\n");
        return ERROR;
    }

    size_t region_size = NUM_OF_PAGES * page_size;
    void *region = mmap(NULL, region_size, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (region == MAP_FAILED) {
        perror("Error with mmap in anonym_mem_game function.\n");
        return ERROR;
    }
    printf("10 pages of anonymous memory allocated.\n\n");

    printf("Attempted writing to the prohibited region.\n");
    if (sigsetjmp(jump_buffer, 1) == 0) {
        ((char*)region)[0] = 2;
    } 

    sleep(2);

    int is_error = mprotect(region, region_size, PROT_NONE);
    if (is_error == -1) {
        perror("Error with mprotect in anonym_mem_game function.\n");
        return ERROR;
    }
    printf("\nPermissions changed to deny reading.\n");

    printf("Attempted reading from a prohibited region.\n");
    if (sigsetjmp(jump_buffer, 1) == 0) {
         printf("%c\n", ((char *)region)[0]);
    }

    printf("\nPress 'Enter' if you want to munmap 4 to 6 memory pages.\n");
    catch_enter();

    void *unmap_start = (char *)region + START_PAGE_TO_MUNMAP * page_size;
    is_error = munmap(unmap_start, NUM_OF_PAGES_TO_MUNMAP * page_size);
    if (is_error == ERROR) {
        perror("Error with munmap in anonym_mem_game function.\n");
        return ERROR;
    }
    printf("Memory released.\n");

    printf("Press 'Enter' if you want to munmap all memory.\n");
    catch_enter();

    is_error = munmap(region, region_size);
    if (is_error == ERROR) {
        perror("Error with munmap in anonym_mem_game function.\n");
        return ERROR;
    }
    printf("Memory released.\n");
    return 0;
}


int experiment(){
    pid_t pid = getpid();
    printf("PID: %d\n", pid);

    printf("Press 'Enter' if you are ready.\n"); 
    catch_enter();
    int is_error = array_alloc_on_stack();
    if (is_error == ERROR)
        return ERROR;

    printf("Press 'Enter' if you want to continue.\n");
    
    catch_enter();
    is_error = array_alloc_in_heap();
    if (is_error == ERROR)
        return ERROR;
    
    printf("Press 'Enter' if you want to test anonymous memory.\n");
    catch_enter();
    is_error = anonym_mem_game();
    if (is_error == ERROR)
        return ERROR;

    return NO_ERROR;
}

