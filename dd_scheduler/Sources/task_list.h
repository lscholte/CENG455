/*
 * task_list.h
 *
 *  Created on: Mar 6, 2017
 *      Author: lscholte
 */

#ifndef SOURCES_TASK_LIST_H_
#define SOURCES_TASK_LIST_H_

#include <stdint.h>

typedef struct task_node {
	uint32_t tid;
	uint32_t absolute_deadline;
	uint32_t creation_time;
	struct task_node *next_node;
	struct task_node *previous_node;
} ACTIVE_TASK_NODE, OVERDUE_TASK_NODE, TASK_NODE;

typedef struct {
	TASK_NODE *head;
} ACTIVE_TASK_LIST, OVERDUE_TASK_LIST, TASK_LIST;


void addToFront(TASK_LIST *task_list, TASK_NODE *node_ptr);
void remove(TASK_LIST *task_list, TASK_NODE *node_ptr);
void copy(TASK_LIST *original, TASK_LIST *copied_list);
void destroyList(TASK_LIST *list);
#endif /* SOURCES_TASK_LIST_H_ */
