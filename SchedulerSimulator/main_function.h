//
// Created by Stefano Pirastru on 05/11/18.
//
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef STUDENT_MAIN_FUNCTION_H
#define STUDENT_MAIN_FUNCTION_H
#include "structures.h"


void printList(c_list * l);

//prints tasks contained in a TCB struct
void printTask(TCB * t);

//insert link at the first location
void insertf(int inst_l,int inst_flag, c_list *lis);

//insert link at the last location
void insertl(instruction i, c_list *lis);

int countTOccurrences(FILE *fptr);


TCB* importTasks(char* in_file);
#endif //STUDENT_MAIN_FUNCTION_H
