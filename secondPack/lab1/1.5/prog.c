#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define SUCCESS 0
#define ERROR -1
#define SLEEP_TIME 1000
#define TRUE 1


void my_sigint_handler(int signum)
{
    if (signum != SIGINT)
        return;
    const char msg[] = "sigint handler: the SIGINT was caught\n";
    write(STDOUT_FILENO, msg, sizeof(msg));
}


void* thread_with_handler()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = my_sigint_handler;
    int err = sigaction(SIGINT, &sa, NULL);
    if (err != SUCCESS) {
        printf("thread_with_handler: sigaction failed: %s\n", strerror(errno));
        return (void*)ERROR;
    }

    sigset_t mask;
    sigemptyset(&mask);
    err = sigaddset(&mask, SIGQUIT);
    if (err != SUCCESS) {
        printf("thread_with_handler: sigaddset failed: %s\n", strerror(errno));
        return (void*)ERROR;
    }
    err = pthread_sigmask(SIG_BLOCK, &mask, NULL);
    if (err != SUCCESS) {
        printf("thread_with_handler: pthread_sigmask failed: %s\n", strerror(err));
        return (void*)ERROR;
    }

    printf("thread_with_handler [pid %d tid %d]: \n", getpid(), gettid());
    //while (TRUE)
        usleep(SLEEP_TIME);
    return NULL;
}


void* thread_sigwait()
{
    int err;
    int sig = 0;

    sigset_t waitset;
    sigemptyset(&waitset);
    sigaddset(&waitset, SIGQUIT);

    printf("thread_sigwait [pid %d tid %d]: waiting for SIGQUIT\n", getpid(), gettid());
    err = sigwait(&waitset, &sig);
    if (err != SUCCESS) {
        printf("thread_sigwait: sigwait failed: %s\n", strerror(err));
        return (void*)ERROR;
    }
    printf("thread_sigwait [pid %d tid %d]: sigwait received signal %d\n", getpid(), gettid(), sig);
    return NULL;
}


int main()
{
    pthread_t t1, t2;

    int err = pthread_create(&t1, NULL, thread_with_handler, NULL);
    if (err != SUCCESS) {
        printf("main: pthread_create(t1) failed: %s\n", strerror(err));
        return ERROR;
    }

    err = pthread_create(&t2, NULL, thread_sigwait, NULL);
    if (err != SUCCESS) {
        printf("main: pthread_create(t2) failed: %s\n", strerror(err));
        return ERROR;
    }

    sigset_t all;
    sigfillset(&all);

    err = pthread_sigmask(SIG_BLOCK, &all, NULL);
    if (err != SUCCESS) {
        printf("thread_block_all: pthread_sigmask failed: %s\n", strerror(err));
        return ERROR;
    }

    printf("pid: %d\nthread_block_all: %d\n", getpid(), gettid());

    while (TRUE)
        usleep(SLEEP_TIME);
    
    return SUCCESS;
}