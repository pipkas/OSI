#include <unistd.h>

ssize_t my_syscall(const char* message) {
    ssize_t ret;
    asm volatile (
        "mov $1, %%rax\n"   
        "mov $1, %%rdi\n"   
        "mov %1, %%rsi\n"   
        "mov $12, %%rdx\n"  
        "syscall\n"       
        "mov %%rax, %0\n"  
        : "=r" (ret)       
        : "r" (message)    
        : "%rax", "%rdi", "%rsi", "%rdx", "%rcx", "%r11" 
    );
    return ret;
}
