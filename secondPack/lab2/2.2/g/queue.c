#define _GNU_SOURCE
#include <pthread.h>
#include <assert.h>

#include "queue.h"

void *qmonitor(void *arg) {
	queue_t *q = (queue_t *)arg;
	int err;
	printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

	while (1) {
		err = sem_wait(&q->queue_access);
		if (err != SUCCESS) {
			perror("qmonitor: sem_wait failed\n");
			continue;
		}
        queue_print_stats(q);
        err = sem_post(&q->queue_access);
		if (err != SUCCESS)
			perror("qmonitor: sem_post failed\n");
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

	err = sem_init(&q->available_slots, PRIVATE, max_count);
    if (err != SUCCESS) {
        perror ("queue_init: sem_init failed\n");
        free(q);
        return NULL;
    }

	err = sem_init(&q->available_items, PRIVATE, 0);
	if (err != SUCCESS) {
		perror ("queue_init: sem_init failed\n");
		err = sem_destroy(&q->available_slots);
		if (err != SUCCESS)
			perror ("queue_init: destroy failed\n");
		free(q);
		return NULL;
	}

	err = sem_init(&q->queue_access, PRIVATE, ALLOWED);
	if (err != SUCCESS) {
		perror ("queue_init: sem_init failed\n");
		err = sem_destroy(&q->available_slots);
		if (err != SUCCESS)
			perror ("queue_init: destroy failed\n");
		err = sem_destroy(&q->available_items);
		if (err != SUCCESS)
			perror ("queue_init: destroy failed\n");
		free(q);
		return NULL;
	}

	err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
	if (err != SUCCESS) {
		perror ("queue_init: pthread_create failed\n");
		err = sem_destroy(&q->available_slots);
		if (err != SUCCESS)
			perror ("queue_init: destroy failed\n");
		err = sem_destroy(&q->available_items);
		if (err != SUCCESS)
			perror ("queue_init: destroy failed\n");
		err = sem_destroy(&q->queue_access);
		if (err != SUCCESS)
			perror ("queue_init: destroy failed\n");
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
	err = sem_destroy(&q->available_slots);
	if (err != SUCCESS)
		perror ("queue_destroy: destroy failed\n");
	err = sem_destroy(&q->available_items);
	if (err != SUCCESS)
		perror ("queue_destroy: destroy failed\n");
	err = sem_destroy(&q->queue_access);
	if (err != SUCCESS)
		perror ("queue_destroy: destroy failed\n");
		
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

	int err = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    if (err != SUCCESS) {
        perror("queue_add: pthread_setcancelstate failed\n");
		free(new);
    	return FAIL;
    }

	err = sem_wait(&q->available_slots);
	if (err != SUCCESS) {
		perror("queue_add: sem_wait failed\n");
		free(new);
		err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		if (err != SUCCESS)
			perror("queue_add: pthread_setcancelstate failed\n");
		return FAIL;
	}
	err = sem_wait(&q->queue_access);
	if (err != SUCCESS) {
		perror("queue_add: sem_wait failed\n");
		free(new);
		err = sem_post(&q->available_slots);
		if (err != SUCCESS)
			perror("queue_add: sem_post failed\n");
		err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		if (err != SUCCESS)
			perror("queue_add: pthread_setcancelstate failed\n");
		return FAIL;
	}
	q->add_attempts++;	

	if (q->first == NULL)
    	q->first = q->last = new;
	else {
    	q->last->next = new;
    	q->last = new;
	}
	
	q->count++;
	q->add_count++;

	err = sem_post(&q->queue_access);
    if (err != SUCCESS)
		perror("queue_add: sem_post failed\n");
    err = sem_post(&q->available_items);
    if (err != SUCCESS){
        perror("queue_add: sem_post failed\n");
	}

	err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (err != SUCCESS) {
        perror("queue_add: pthread_setcancelstate failed\n");
        return FAIL;
    }

	return SUCCESS;
}

int queue_get(queue_t *q, int *val) {
	int err = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    if (err != SUCCESS) {
        perror("queue_get: pthread_setcancelstate failed\n");
        return ERROR;
    }

	err = sem_wait(&q->available_items);
	if (err != SUCCESS) {
		perror("queue_get: sem_wait failed\n");
		err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		if (err != SUCCESS)
			perror("queue_get: pthread_setcancelstate failed\n");
		return FAIL;
	}
	err = sem_wait(&q->queue_access);
	if (err != SUCCESS) {
		perror("queue_get: sem_wait failed\n");
		err = sem_post(&q->available_slots);
		if (err != SUCCESS)
			perror("queue_get: sem_post failed\n");
		err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		if (err != SUCCESS)
			perror("queue_get: pthread_setcancelstate failed\n");
		return FAIL;
	}

	q->get_attempts++;

	qnode_t *tmp = q->first;
	*val = tmp->val;
	q->first = q->first->next;
	if (q->first == NULL)
        q->last = NULL;
	free(tmp);
	q->count--;
	q->get_count++;

	err = sem_post(&q->queue_access);
    if (err != SUCCESS)
		perror("queue_get: sem_post failed\n");
    err = sem_post(&q->available_slots);
    if (err != SUCCESS){
        perror("queue_get: sem_post failed\n");
	}

    err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (err != SUCCESS) {
        perror("queue_get: pthread_setcancelstate failed\n");
        return ERROR;
    }

	return SUCCESS;
}

void queue_print_stats(queue_t *q) {
	printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
		q->count,
		q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
		q->add_count, q->get_count, q->add_count -q->get_count);
}
