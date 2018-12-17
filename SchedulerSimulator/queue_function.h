//
// Created by Stefano Pirastru on 17/11/18.
//

#ifndef STUDENT_QUEUE_FUNCTION_H
#define STUDENT_QUEUE_FUNCTION_H

#include "structures.h"

void print_queue(job_q p);
void insert_bottom(job_q p, job n);
job search_node_by_task_id(job_q p, int desc);
void delete_node(job_q p, int desc);
job create_node(TCB* desc);


#endif //STUDENT_QUEUE_FUNCTION_H
