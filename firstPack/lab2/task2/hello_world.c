#include "my_syscall.h"

int main(){
    const char* message = "Hello World\n";
    my_syscall(message);
    return 0;
}
