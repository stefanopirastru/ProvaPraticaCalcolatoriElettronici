//
// Created by Stefano Pirastru on 17/11/18.
//
#ifndef _Q_FUNCTIONS
#define _Q_FUNCTIONS

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

#include "structures.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void print_queue(job_q p){
    int i = 0;

    pthread_mutex_lock(&lock); //lock the queue

    printf("v~~top\n");
    job tmp = p->first;
    while(i<p->number){
        printf("%d: %d\n",++i, tmp->desc->id);
        tmp = tmp->next;
    }
    printf("---bottom\n");
    i = 0;
    tmp = p->last;
    while(NULL != tmp){
        printf("%d: %d\n", ++i, tmp->desc->id);
        tmp = tmp->prev;
    }
    printf("v~~top\n\n");

    pthread_mutex_unlock(&lock); //picklock
}

void insert_bottom(job_q p, job n) //task enqueued
{
    n->next = n->prev = NULL;
    if (NULL == p->first) {
        p->first = n;
        p->last = n;
        return;
    }
    p->last->next = n;
    n->prev = p->last;
    p->last = n;
    p->number++;
}

job search_node_by_task_id(job_q p, int desc){ //search element using the task id
    job tmp = p->first;
    while(NULL != tmp){
        if(tmp->desc->id == desc) return tmp;
        tmp = tmp->next;
    }
    return NULL;
}

void delete_node(job_q p, int desc){
    job del = search_node_by_task_id(p, desc);
    if(NULL == del) return;
    if(del == p->first){ //first case, I have to cancel the first
        p->first = del->next;
        if(del->next != NULL) del->next->prev = NULL;
        del->next = NULL;
    }
    if(del == p->last){ //second case, I have to cancel the last
        p->last = del->prev;
        if(del->prev != NULL) del->prev->next = NULL;
        del->prev = NULL;
    }
    if(NULL != del->next){ //third case, I have to cancel in the middle
        del->next->prev = del->prev;
    }
    free(del); //deallocating space
    if(p->number > 0) p->number--;
}

job create_node(TCB* desc){
    job new_node = (job) malloc(sizeof(job_n));
    new_node->desc = (TCB*) malloc(sizeof(TCB));
    new_node->desc = desc;
    return new_node;
}



#endif
