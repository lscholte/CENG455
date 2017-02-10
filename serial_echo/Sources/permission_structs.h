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
	_queue_number stream_no;
} READ_PRIVILEGE, * READ_PRIVILEGE_PTR;


#endif /* SOURCES_PERMISSION_STRUCTS_H_ */
