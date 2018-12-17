#ifndef STUDENT_MAIN_STRUCT_H
#define STUDENT_MAIN_STRUCT_H


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

enum states {news, ready, running, blocked, iexit} states;

//instruction type definition;
typedef struct instruction{
    int type_flag;
    int length;

} instruction;

//instruction type definition;
typedef struct node {
    instruction data;
    struct node *next;
}node;

//list type definition
typedef struct c_list{
    int length;
    struct node *head;
}c_list;

//Task Control Block type definition;
typedef struct TCB{
    int id;
    instruction * pc;
    int arrival_time;
    c_list instr_list;
    enum states instr_state;

}TCB;

//node for scheduling tasks
typedef struct job_node *job;
typedef struct job_node{
    TCB *desc;
    job next;
    job prev;
}job_n;

//queue for buffering tasks
typedef struct job_queue *job_q;
typedef struct job_queue{
    int number;
    job first;
    job last;
}Queue;

//context for thread
typedef struct wrapper{
    job_q jobs;
    FILE* log;
    int who;
    pthread_mutex_t* lock;
    sem_t full;
    sem_t empty;
}wrap;

#endif