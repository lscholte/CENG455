/*
 * task_list.h
 *
 *  Created on: Mar 6, 2017
 *      Author: lscholte
 */

#ifndef SOURCES_TASK_LIST_H_
#define SOURCES_TASK_LIST_H_

typedef struct task_node {
	uint32_t tid;
	uint32_t absolute_deadline;
	uint32_t task_type;
	uint32_t creation_time;
	struct task_node *next_node;
	struct task_node *previous_node;
} ACTIVE_TASK_NODE, OVERDUE_TASK_NODE, TASK_NODE;

typedef struct {
	TASK_NODE *head;
} ACTIVE_TASK_LIST, OVERDUE_TASK_LIST, TASK_LIST;

#endif /* SOURCES_TASK_LIST_H_ */
