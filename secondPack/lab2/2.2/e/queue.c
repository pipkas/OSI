#define _GNU_SOURCE
#include <pthread.h>
#include <assert.h>

#include "queue.h"

void *qmonitor(void *arg) {
	queue_t *q = (queue_t *)arg;
	int err;
	printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

	while (1) {
		err = pthread_mutex_lock(&q->lock);
		if (err != SUCCESS) {
			perror("qmonitor: pthread_mutex_lock failed\n");
			continue;
		}
        queue_print_stats(q);
        err = pthread_mutex_unlock(&q->lock);
		if (err != SUCCESS)
			perror("qmonitor: pthread_mutex_unlock failed\n");
		sleep(MONITOR_INTERVAL);
	}

	return NULL;
}

queue_t* queue_init(int max_count) {
	int err;

	queue_t *q = malloc(sizeof(queue_t));
	if (q == NULL) {
		printf("Cannot allocate memory for a queue\n");
		return NULL;
	}

	q->first = NULL;
	q->last = NULL;
	q->max_count = max_count;
	q->count = 0;

	q->add_attempts = q->get_attempts = 0;
	q->add_count = q->get_count = 0;

	err = pthread_mutex_init(&q->lock, NULL);
    if (err != SUCCESS) {
        printf("queue_init: pthread_mutex_init failed: %s\n", strerror(err));
        free(q);
        return NULL;
    }

	err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
	if (err != SUCCESS) {
		printf("queue_init: pthread_create() failed: %s\n", strerror(err));
		err = pthread_mutex_destroy(&q->lock);
        if (err != SUCCESS)
            printf("queue_init: pthread_mutex_destroy failed: %s\n", strerror(err));
        free(q);
		return NULL;
	}

	return q;
}


int queue_destroy(queue_t *q) {
	if (q == NULL) 
		return ERROR;
	int err;
    err = pthread_cancel(q->qmonitor_tid);
	if (err != SUCCESS){
        perror("queue_destroy: pthread_cancel() failed\n");
        return ERROR;
    }
	err = pthread_join(q->qmonitor_tid, NULL);
    if (err != SUCCESS){
        perror("queue_destroy: pthread_join() failed\n");
        return ERROR;
    }
	err = pthread_mutex_destroy(&q->lock);
    if (err != SUCCESS){
        perror("queue_destroy: pthread_mutex_destroy() failed\n");
        return ERROR;
    }
    qnode_t *current = q->first;
    while (current != NULL) {
        qnode_t *temp = current;
        current = current->next;
        free(temp);
    }
    free(q);
    return SUCCESS;
}

int queue_add(queue_t *q, int val) {

	qnode_t *new = malloc(sizeof(qnode_t));
	if (new == NULL) {
		printf("Cannot allocate memory for new node\n");
		return FAIL;
	}
	new->val = val;
	new->next = NULL;

	int err = pthread_mutex_lock(&q->lock);
	if (err != SUCCESS) {
		perror("queue_add: pthread_mutex_lock failed\n");
		free(new);
		return FAIL;
	}
	q->add_attempts++;	

	if (q->count == q->max_count){
		err = pthread_mutex_unlock(&q->lock);
		free(new);
		if (err != SUCCESS)
			perror("queue_add: pthread_mutex_unlock failed\n");
		return FAIL;
	}

	if (q->first == NULL)
    	q->first = q->last = new;
	else {
    	q->last->next = new;
    	q->last = new;
	}
	
	q->count++;
	q->add_count++;
	err = pthread_mutex_unlock(&q->lock);
	if (err != SUCCESS)
		perror("queue_add: pthread_mutex_unlock failed\n");
	return SUCCESS;
}

int queue_get(queue_t *q, int *val) {
	int err = pthread_mutex_lock(&q->lock);
	if (err != SUCCESS) {
		perror("queue_get: pthread_mutex_lock failed\n");
		return FAIL;
	}

	q->get_attempts++;

	if (q->count == 0){
		err = pthread_mutex_unlock(&q->lock);
		if (err != SUCCESS)
			perror("queue_get: pthread_mutex_lock failed\n");
		return FAIL;
	}

	qnode_t *tmp = q->first;
	*val = tmp->val;
	q->first = q->first->next;
	if (q->first == NULL)
        q->last = NULL;
	free(tmp);
	q->count--;
	q->get_count++;

	err = pthread_mutex_unlock(&q->lock);
	if (err != SUCCESS)
		perror("queue_get: pthread_mutex_unlock failed\n");
	return SUCCESS;
}

void queue_print_stats(queue_t *q) {
	printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
		q->count,
		q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
		q->add_count, q->get_count, q->add_count -q->get_count);
}
