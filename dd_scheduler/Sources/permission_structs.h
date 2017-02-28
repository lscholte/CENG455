/*
 * permission_structs.h
 *
 *  Created on: Feb 10, 2017
 *      Author: lscholte
 */

#ifndef SOURCES_PERMISSION_STRUCTS_H_
#define SOURCES_PERMISSION_STRUCTS_H_

typedef struct write_privilege {
	uint32_t task_id; //if this has a value of 0, then no task has write privileges
	_queue_id qid; //this will be initially set by the handler task and then never modified again
} WRITE_PRIVILEGE, * WRITE_PRIVILEGE_PTR;

typedef struct read_privilege {
	uint32_t task_id;
	_queue_id qid;
} READ_PRIVILEGE, * READ_PRIVILEGE_PTR;

typedef struct task_node {
	uint32_t tid;
	uint32_t deadline;
	uint32_t task_type;
	uint32_t creation_time;
	struct task_node *next_node;
	struct task_node *previous_node;
} ACTIVE_TASK_NODE, OVERDUE_TASK_NODE, TASK_NODE;

typedef struct {
	TASK_NODE *head;
} ACTIVE_TASK_LIST, OVERDUE_TASK_LIST;


#endif /* SOURCES_PERMISSION_STRUCTS_H_ */
