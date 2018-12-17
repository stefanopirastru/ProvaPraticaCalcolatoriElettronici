//
// Created by Stefano Pirastru on 05/11/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "main_function.h"
#include "structures.h"

#ifndef _MAIN_
#define _MAIN_






//display the instructions list
void printList(c_list * l) {

    struct node *ptr = l->head;

    printf("\n[head] =>");
    //start from the beginning
    while(ptr != NULL) {
        printf("Instruction %d  %d =>",ptr->data.length,ptr->data.type_flag);
        ptr = ptr->next;
    }

    printf(" [null]\n");
}

//prints tasks contained in a TCB struct
void printTask(TCB * t)
{
    if(t) {
        if (t->instr_list.length > 1) {
            printf("TASK ID %d\n", t->id);
            printf("TASK ARRIVAL TIME %d\n", t->arrival_time);
            printf("TASK INSTRUCTION LIST");
            printf("\nlength %d", t->instr_list.length);
            printList(&t->instr_list);
        }
    }
}

//insert link at the first location
void insertf(int inst_l,int inst_flag, c_list *lis) {
    //create a link
    node *link = malloc(sizeof(node));

    instruction i;
    i.length=inst_l;
    i.type_flag=inst_flag;

    //link->key = key;
    link->data = i;

    //point it to old first node
    link->next = lis->head;

    //point first to new first node
    lis->head = link;
}

//insert link at the last location
void insertl(instruction i, c_list *lis) {
    //create a link
    node *link = malloc(sizeof(node));


    //set data and nextpointer;
    link->data = i;
    link->next=NULL;
    if(lis->head==NULL){
        lis->head=link;
        lis->length=0;
    }

    else{
        node *pointer=lis->head;

        //traverse till last node
        while(pointer->next!=NULL)
        {
            pointer=pointer->next;
        }
        pointer->next=link;
        lis->length++;
    }

}


int countTOccurrences(FILE *fptr)
{
    int count = 0;
    char c = '\0';
    while (c!=EOF)
    {
        c = fgetc(fptr);
        if(isalpha(c) && c=='t')
        {
            count ++;
        }
    }
    return count;
}


TCB* importTasks(char* in_file){
    FILE *fptr;
    char c = '\0';
    int slot=-1;
    char numb[10];
    int numl=0;
    int first=1;

    instruction* itemp = NULL;


    int iort=0;

    /* Try to open file */
    fptr = fopen(in_file, "r");

    if (fptr == NULL)
    {
        printf("Unable to open file.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("File aperto\n");
    }


    int occ=countTOccurrences(fptr);
    //allocates memory for all the Task Control Blocks needed
    TCB* tasksArray=malloc(sizeof(TCB)*occ);

    TCB* itor=tasksArray;


    rewind(fptr);

    do {


        c = fgetc(fptr);
        if( feof(fptr) )
            break ;
        //printf("%c\n",c);

        //Reading a new task
        if (isalpha(c) && c=='t')
        {
            slot=-1;
            iort=1;
            if(first==1)
            {
                first=0;
            }
            else
            {
                itor++;
            }
        }

        //Reading a new instruction
        if (isalpha(c) && c=='i')
        {	slot=-1;
            iort=0;
            //Creates a new empty instruction to be filled and linked in current task list
            itemp=malloc(sizeof(instruction));

        }


        //Reading a number (a field of task or instruction,var iort tells me which)
        if( isdigit(c))
        {

            numb[numl]=c;
            numl++;
        }
        else{
            if (numl!=0)
            {
                slot++;
                //printf("Slot= %d\n",slot);
                numb[numl]='\0';
                numl=0;
                int n=atoi(numb);
                //case in which i must put number i find in the task
                if(iort==1)
                {

                    if(slot==0)
                    {
                        itor->id=n;
                    }
                    else
                    {
                        itor->arrival_time = n;
                        itor->instr_state = news;
                    }

                }
                //case in which i must put number i find in the instructions list
                if(iort==0)
                {
                    if(slot==0)
                    {

                        itemp->type_flag=n;
                    }
                    else
                    {

                        itemp->length=n;
                        insertl(*itemp, &itor->instr_list);
                    }
                }

            }
        }

    } while(1);

    fclose(fptr);



    return tasksArray;
}

#endif



