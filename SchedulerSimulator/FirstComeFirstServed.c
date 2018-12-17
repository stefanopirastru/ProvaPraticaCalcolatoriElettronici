//
// Created by Stefano Pirastru on 25/11/18.
//

#include <pthread.h> //thread
#include <stdio.h> //printf fprintf
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <semaphore.h>
#include "structures.h"
#include "queue_function.h"
#include "main_function.h"




void tempo(job_q ready, job_q blocked, struct wrapper *inside){
    job bookmark = blocked->first;
    int flag = 1;
    while(bookmark != NULL && flag){

        TCB* unit = bookmark->desc;
        if(unit!= NULL) {
            pthread_mutex_trylock(inside->lock);
            if (unit->pc != NULL) {
                unit->pc->length--;
                pthread_mutex_unlock(inside->lock);
            }

        }
        if(!unit) break;
        if(unit->pc == NULL) break;
        if(!bookmark->desc) break;
        if(unit->pc->length == 0){
            job consumed = bookmark;
            bookmark = consumed->next;
            insert_bottom(ready, create_node(unit));
            delete_node(blocked, unit->id);
        }else{
            bookmark = bookmark->next;
            if(!bookmark) break;
        }
    }
}

int randomize(int target){
    int those = 0;
    if(target != 0) {
        srand((unsigned int) time(NULL));
        those = rand() % target + 1; //in range 1 to length
        printf("%d\n", those);
        return those;
    }
    return 0;
}


void* firstComeFirstServed(void* arg){
    struct wrapper *inside = (struct wrapper*) arg;

    job tmp = inside->jobs->first;
    Queue readyQueue, blockedQueue; //queue of tasks
    instruction *atom = NULL;
    struct node *nore = NULL;
    TCB* alu = NULL;
    readyQueue.first = readyQueue.last = blockedQueue.first = blockedQueue.last = NULL; //initialize new queues
    unsigned int clock = 0;
    int done = 1;
    int result;
    while(done) {

        while (tmp != NULL && tmp->desc->arrival_time <= clock) {
            //admit

            pthread_mutex_trylock(inside->lock);
            printf("lock thread %d\n", inside->who);
            if(tmp->desc->instr_state == news) {
                tmp->desc->instr_state = ready;
                fprintf(inside->log, "core%d,%d,%d,ready\n", inside->who, tmp->desc->id, clock);
                insert_bottom(&readyQueue, create_node(tmp->desc)); //enqueued node in the last ready queue
                tmp = tmp->next;
                pthread_mutex_unlock(inside->lock);
                printf("unlock thread %d after getting ready\n ", inside->who);
            }else{
                tmp = tmp->next;
                pthread_mutex_unlock(inside->lock);
                printf("unlock thread %d while are not ready\n", inside->who);
            }

        }

        //pthread_mutex_unlock(inside->lock);
        if(readyQueue.first == NULL && blockedQueue.first == NULL && atom == NULL && alu == NULL && tmp == NULL) break;

        //dispatch
        if(alu == NULL){
            if(readyQueue.first != NULL) {
                pthread_mutex_trylock(inside->lock);
                printf("lock thread %d dispatching\n ", inside->who);
                alu = readyQueue.first->desc; //first served
                pthread_mutex_trylock(inside->lock);
                delete_node(&readyQueue, readyQueue.first->desc->id);
                pthread_mutex_unlock(inside->lock);
                alu->instr_state = running;
                fprintf(inside->log, "core%d,%d,%d,running\n", inside->who, alu->id, clock);
                pthread_mutex_unlock(inside->lock);
                printf("lock thread %d after dispatching\n ", inside->who);
            }
            if(alu == NULL){
                clock++;
                tempo(&readyQueue, &blockedQueue, inside);
                continue;
            }

            if(alu->pc == NULL){
                nore = alu->instr_list.head;
                atom = &nore->data;
                if(atom) goto CONSUME;
            }
        }

        if(alu != NULL && (alu->pc->length == 0) && alu->pc != 0x1){
            if(nore == NULL){
                printf("at time %d nore is null\n", clock);
                nore = alu->instr_list.head;
                for(int i = 0; i < alu->instr_list.length; i++){
                    printf("%d\n",i);
                    if(nore->data.length == 0) nore = nore->next;
                    else i = alu->instr_list.length;
                }
            }
            if(nore->next == NULL){
                printf("lock thread %d finished task\n ", inside->who);
                pthread_mutex_trylock(inside->lock);
                alu->instr_state = iexit;
                pthread_mutex_unlock(inside->lock);
                fprintf(inside->log, "core%d,%d,%d,exit\n", inside->who, alu->id, clock);
                alu = NULL;
                clock++;
                tempo(&readyQueue, &blockedQueue, inside);

                printf("unlock thread %d finished task\n ", inside->who);
                continue;
            }else{
                nore = nore->next;
                atom = &nore->data;
            }
        }else{
            atom = alu->pc;
        }

        CONSUME:
        if(atom) alu->pc = atom;
        if(!alu->pc->length) {
            printf("%d\n",alu->id);
            fprintf(stderr, "This instruction contain %c\n", alu->pc->length);
        }

        if(alu->instr_state != running){
            pthread_mutex_trylock(inside->lock);
            printf("lock thread %d running\n ", inside->who);
            alu->instr_state = running;
            fprintf(inside->log, "core%d,%d,%d,running\n", inside->who, alu->id, clock);
            pthread_mutex_unlock(inside->lock);
            printf("unlock thread %d running\n ", inside->who);
        }

        if(atom->type_flag == 0){
            pthread_mutex_trylock(inside->lock);
            printf("lock thread %d nonblock\n ", inside->who);
            atom->length--;
            if(atom->length > 0){
                clock++;
                tempo(&readyQueue, &blockedQueue, inside);
                pthread_mutex_unlock(inside->lock);
                printf("unlock thread %d non finished nonblocking\n ", inside->who);
                continue;
            }

            if(nore->next != NULL){
                nore = nore->next;
                atom = &nore->data;
            }else{
                pthread_mutex_trylock(inside->lock);
                printf("lock thread %d exit 2\n ", inside->who);
                alu->instr_state = iexit;
                fprintf(inside->log, "core%d,%d,%d,exit\n", inside->who, alu->id, clock);
                alu = NULL;
                nore = NULL;
                atom = NULL;
                pthread_mutex_unlock(inside->lock);
                printf("unlock thread %d exit 2\n ", inside->who);
            }
            clock++;
            tempo(&readyQueue, &blockedQueue, inside);
            continue;
        }

        if(atom->type_flag == 1){
            printf("lock thread %d blocking\n ", inside->who);
            atom->length = randomize(atom->length);
            if(alu->pc == NULL){
                fprintf(stderr, "NOT THIS WAY!!!\n");
                exit(EXIT_FAILURE);
            }
            pthread_mutex_trylock(inside->lock);
            alu->instr_state = blocked;
            pthread_mutex_unlock(inside->lock);
            fprintf(inside->log, "core%d,%d,%d,blocked\n", inside->who, alu->id, clock);
            insert_bottom(&blockedQueue, create_node(alu));
            alu = NULL;
            atom = NULL;

            printf("lock thread %d after blocking\n ", inside->who);
        }
        clock++;
        tempo(&readyQueue, &blockedQueue, inside);

    }
}