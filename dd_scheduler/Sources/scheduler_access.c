/*
 * scheduler_access.c
 *
 *  Created on: Mar 18, 2017
 *      Author: sat
 */


#include <stdint.h>
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"
#include <message.h>
#include "task_list.h"
#include "message_structs.h"
#include "scheduler_access.h"

/*
 * Exit handler that is attached to every scheduled task
 */
void exitHandler() {
	dd_delete(_task_get_id());
}

/*
 * Creates a new task and sends a message to the deadline-driven
 * scheduler to scheduler a new task
 */
_task_id dd_tcreate(uint32_t template_index, uint32_t deadline) {

	_task_id tid = _task_create(0, template_index, (uint32_t)(NULL));

	GENERIC_MESSAGE_PTR msg_ptr = (GENERIC_MESSAGE_PTR)_msg_alloc(message_pool);
	if (msg_ptr == NULL){
		printf("\nCould not allocate a message\n");
		_task_block();
	}

	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, SCHEDULER_QUEUE);
	msg_ptr->HEADER.SOURCE_QID = MSGQ_NULL_QUEUE_ID;
	msg_ptr->HEADER.SIZE = sizeof(GENERIC_MESSAGE);

	TASK_CREATION_DATA *data_ptr = malloc(sizeof(TASK_CREATION_DATA));
	data_ptr->TASK_ID = tid;
	data_ptr->DEADLINE = deadline;

	msg_ptr->TYPE = TASK_CREATION_MESSAGE_TYPE;
	msg_ptr->DATA_PTR = data_ptr;

	bool result = _msgq_send(msg_ptr);
	if (result != TRUE) {
		printf("\nCould not send a message\n");
		return 0;
	}

	_task_set_exit_handler(tid, exitHandler);

	return tid;
}

/*
 * Effectively a copy of dd_tcreate(template_index, deadline)
 * but with an extra parameter, runtime. The runtime parameter is used as the total
 * time in a synthetic compute function for testing the scheduler with fake tasks that
 * take a predefined amount of time to execute.
 */
_task_id dd_tcreate_test(uint32_t template_index, uint32_t deadline, uint32_t runtime) {

	_task_id tid = _task_create(0, template_index, runtime);

	GENERIC_MESSAGE_PTR msg_ptr = (GENERIC_MESSAGE_PTR)_msg_alloc(message_pool);
	if (msg_ptr == NULL){
		printf("\nCould not allocate a message\n");
		_task_block();
	}

	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, SCHEDULER_QUEUE);
	msg_ptr->HEADER.SOURCE_QID = MSGQ_NULL_QUEUE_ID;
	msg_ptr->HEADER.SIZE = sizeof(GENERIC_MESSAGE);

	TASK_CREATION_DATA *data_ptr = malloc(sizeof(TASK_CREATION_DATA));
	data_ptr->TASK_ID = tid;
	data_ptr->DEADLINE = deadline;

	msg_ptr->TYPE = TASK_CREATION_MESSAGE_TYPE;
	msg_ptr->DATA_PTR = data_ptr;

	bool result = _msgq_send(msg_ptr);
	if (result != TRUE) {
		printf("\nCould not send a message\n");
		return 0;
	}

	_task_set_exit_handler(tid, exitHandler);

	return tid;
}

/*
 * Sends a message to the scheduler to delete a given scheduled task
 */
uint32_t dd_delete(_task_id tid) {

	GENERIC_MESSAGE_PTR msg_ptr = (GENERIC_MESSAGE_PTR)_msg_alloc(message_pool);
	if (msg_ptr == NULL){
		printf("\nCould not allocate a message\n");
		return 1;
	}

	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, SCHEDULER_QUEUE);
	msg_ptr->HEADER.SOURCE_QID = MSGQ_NULL_QUEUE_ID;
	msg_ptr->HEADER.SIZE = sizeof(GENERIC_MESSAGE);

	TASK_DELETION_DATA *data_ptr = malloc(sizeof(TASK_DELETION_DATA));
	data_ptr->TASK_ID = tid;

	msg_ptr->TYPE = TASK_DELETION_MESSAGE_TYPE;
	msg_ptr->DATA_PTR = data_ptr;

	bool result = _msgq_send(msg_ptr);
	if (result != TRUE) {
		printf("\nCould not send a message\n");
		return 1;
	}

	return MQX_OK;
}

/*
 * Request a list of tasks from the scheduler
 */
uint32_t dd_return_list(TASK_LIST *list, int type) {

	if (type != ACTIVE_TASK_REQUEST_MESSAGE_TYPE && type != OVERDUE_TASK_REQUEST_MESSAGE_TYPE) {
		printf("\nType must be either an active task or overdue task list\n");
		return 1;
	}

	_queue_id qid = _msgq_open(MSGQ_FREE_QUEUE, 0);
	if(qid == MSGQ_NULL_QUEUE_ID) {
		printf("\nCould not create message queue\n");
		return 1;
	}

	GENERIC_MESSAGE_PTR msg_ptr = (GENERIC_MESSAGE_PTR)_msg_alloc(message_pool);
	if (msg_ptr == NULL){
		printf("\nCould not allocate a message\n");
		return 1;
	}

	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, SCHEDULER_QUEUE);
	msg_ptr->HEADER.SOURCE_QID = qid;
	msg_ptr->HEADER.SIZE = sizeof(GENERIC_MESSAGE);

	msg_ptr->TYPE = type;
	msg_ptr->DATA_PTR = list;


	bool result = _msgq_send(msg_ptr);
	if (result != TRUE) {
		printf("\nCould not send a message\n");
		return 1;
	}

	msg_ptr = _msgq_receive(qid, 0);
	if(msg_ptr != NULL) {
		_msg_free(msg_ptr);
	}
	else {
		return 1;
	}

	if(!_msgq_close(qid)) {
		printf("\nCould not close message queue\n");
		return 1;
	}

	return MQX_OK;
}

/*
 * Requests a list of active tasks from the scheduler
 */
uint32_t dd_return_active_list(ACTIVE_TASK_LIST *list) {
	return dd_return_list(list, ACTIVE_TASK_REQUEST_MESSAGE_TYPE);
}

/*
 * Requests a list of overdue tasks that have exceeded their deadline
 */
uint32_t dd_return_overdue_list(OVERDUE_TASK_LIST *list) {
	return dd_return_list(list, OVERDUE_TASK_REQUEST_MESSAGE_TYPE);
}

/*
 * Requests the total time that the idle task has been executing from the scheduler
 */
uint32_t dd_return_idle_time() {

	_queue_id qid = _msgq_open(MSGQ_FREE_QUEUE, 0);
	if(qid == MSGQ_NULL_QUEUE_ID) {
		printf("\nCould not create message queue\n");
		return 0;
	}

	GENERIC_MESSAGE_PTR msg_ptr = (GENERIC_MESSAGE_PTR)_msg_alloc(message_pool);
	if (msg_ptr == NULL){
		printf("\nCould not allocate a message\n");
		return 0;
	}

	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, SCHEDULER_QUEUE);
	msg_ptr->HEADER.SOURCE_QID = qid;
	msg_ptr->HEADER.SIZE = sizeof(GENERIC_MESSAGE);

	uint32_t idle_time;

	msg_ptr->TYPE = IDLE_TIME_REQUEST_MESSAGE_TYPE;
	msg_ptr->DATA_PTR = &idle_time;

	bool result = _msgq_send(msg_ptr);
	if (result != TRUE) {
		printf("\nCould not send a message\n");
		return 0;
	}

	msg_ptr = _msgq_receive(qid, 0);
	if(msg_ptr != NULL) {
		_msg_free(msg_ptr);
	}
	else {
		return 0;
	}

	if(!_msgq_close(qid)) {
		printf("\nCould not close message queue\n");
		return 0;
	}

	return idle_time;
}
