#pragma once

static const long SEC = 1000000L;
static const int _4KB = 4096;
int test_exec(char* argv[], char* envp[]);
int experiment();

typedef enum return_val{
    NO_ERROR = 0,
    ERROR = -1,
    END = 1,
    NO_END = 2
}return_val;
