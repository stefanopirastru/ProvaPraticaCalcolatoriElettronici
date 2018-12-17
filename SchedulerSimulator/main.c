#include "main_function.h"
#include "structures.h"
#include "main.h"
#include "queue_function.h"
#include "FirstComeFirstServed.h"


#ifndef _C_MAIN
#define _C_MAIN

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> //thread, mutex
#include <semaphore.h> //semaphore


#define PAGE_SIZE 4096
#define STK_SIZE (10 * PAGE_SIZE)

pthread_attr_t attr;
void *stack;

int run(char* ar, char* out)
{

    //the program takes in the absolute file path from command line arguments
    char* in_file=ar;

    //receive data from files and prints it, tasks is a pointer to a memory zone where all of the task data is stored contiguously
    //you can go through tasks with pointers arithmetic (tasks++ takes you to the next task)
    //you can then access all data imported from the file accessing its data: tasks->data_needed (see data names in TCB struct)
    //you can access single instructions in a task by accessing tasks->instr_list and cycling through the list
    FILE* fptr;
    fptr = fopen(in_file, "r");
    if (fptr == NULL)
    {
        printf("Reading Unable to open file %s.\n",in_file);
        exit(EXIT_FAILURE);
    }
    int occ=countTOccurrences(fptr);
    fclose(fptr);
    TCB* tasks= importTasks(in_file);
    printf("file chiuso e tasks inizializzati\n");

    int count=0;
    Queue batch_jobs; //queue of new tasks
    batch_jobs.last = NULL;
    batch_jobs.first = NULL;
    batch_jobs.number = 0;
    while(count < occ){
        insert_bottom(&batch_jobs, create_node(tasks));
        //printTask(tasks);
        tasks++;
        count++;
    }
    //print_queue(&batch_jobs);

    wrap item, it;
    FILE* log = fopen(out, "w+"); //truncate
    if(log == NULL){
        printf("Unable to open file %s\n", out);
        exit(EXIT_FAILURE);
    }
    fclose(log);
    log = fopen(out, "a"); //append
    if(log == NULL){
        printf("Unable to open file %s\n", out);
        exit(EXIT_FAILURE);
    }

    item.log = log;
    item.jobs = &batch_jobs;
    pthread_mutexattr_t mutex_attr;
    pthread_mutex_t mute;
    if (pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK) != 0) {
        perror("set mutex attributes fails\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(&mute, &mutex_attr) != 0) {
        perror("initialization of mutex  with attributes fails\n");
        exit(EXIT_FAILURE);
    }

    item.lock = &mute;
    item.who = 1;
    sem_init(&item.empty, 0, 0); //semaphore to communicate void queue
    sem_init(&item.full, 0, occ); //semaphore to communicate full queue
    it = item;
    it.who = 2;

    pthread_t core1;
    posix_memalign(&stack,PAGE_SIZE,STK_SIZE);
    pthread_attr_init(&attr);
    pthread_attr_setstack(&attr,&stack,STK_SIZE);
    pthread_create(&core1, &attr, firstComeFirstServed, &item);

    firstComeFirstServed(&it);

    pthread_join(core1, NULL);
    fclose(log);
    return 0;


}
#endif
