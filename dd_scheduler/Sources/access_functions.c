/*
 * access_functions.c
 *
 *  Created on: Feb 6, 2017
 *      Author: lscholte
 */


#include "access_functions.h"
#include "os_tasks.h"
#include "message_structs.h"
#include <stdio.h>

_task_id dd_tcreate(uint32_t template_index, uint32_t deadline) {

	_queue_id qid = _msgq_open(MSGQ_FREE_QUEUE, 0);
	if(qid == MSGQ_NULL_QUEUE_ID) {
		printf("\nCould not create message queue\n");
		_task_block();
	}

	_task_id tid = _task_create(0, template_index, (uint32_t)(NULL));

	GENERIC_MESSAGE_PTR msg_ptr = (GENERIC_MESSAGE_PTR)_msg_alloc(message_pool);
	if (msg_ptr == NULL){
		printf("\nCould not allocate a message\n");
		_task_block();
	}

	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, SCHEDULER_QUEUE);
	msg_ptr->HEADER.SOURCE_QID = qid;
	msg_ptr->HEADER.SIZE = sizeof(GENERIC_MESSAGE_PTR) + sizeof(int) + sizeof(void*);

	TASK_CREATION_DATA data = { tid, deadline };

	msg_ptr->TYPE = TASK_CREATION_MESSAGE_TYPE;
	msg_ptr->DATA_PTR = &data;

	bool result = _msgq_send(msg_ptr);
	if (result != TRUE) {
		printf("\nCould not send a message\n");
		_task_block();
	}


	//Wait for a response from scheduler so that we know when it is finished
	//with the message data pointer we sent it. Without this msgq_receive,
	//this function will return and the pointer will contain garbage values when
	//the scheduler tries to access it
	msg_ptr = _msgq_receive(qid, 0);
	_msg_free(msg_ptr);

	if(!_msgq_close(qid)) {
		printf("\nCould not close message queue\n");
		_task_block();
	}

	return tid;
}

//Returns
uint32_t dd_delete(_task_id tid) {
	_queue_id qid = _msgq_open(MSGQ_FREE_QUEUE, 0);
	if(qid == MSGQ_NULL_QUEUE_ID) {
		printf("\nCould not create message queue\n");
		_task_block();
	}

//	if(_task_abort(tid) != MQX_OK) {
//		printf("\nCould not abort task\n");
//		return 1;
//	}

	GENERIC_MESSAGE_PTR msg_ptr = (GENERIC_MESSAGE_PTR)_msg_alloc(message_pool);
	if (msg_ptr == NULL){
		printf("\nCould not allocate a message\n");
		_task_block();
	}

	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, SCHEDULER_QUEUE);
	msg_ptr->HEADER.SOURCE_QID = qid;
	msg_ptr->HEADER.SIZE = sizeof(GENERIC_MESSAGE_PTR) + sizeof(int) + sizeof(void*);

//	TASK_DELETION_DATA data = { tid };
	TASK_DELETION_DATA *data_ptr = malloc(sizeof(TASK_DELETION_DATA));
	data_ptr->TASK_ID = tid;

	msg_ptr->TYPE = TASK_DELETION_MESSAGE_TYPE;
	msg_ptr->DATA_PTR = data_ptr;

	bool result = _msgq_send(msg_ptr);
	if (result != TRUE) {
		printf("\nCould not send a message\n");
		return 1;
	}

//	//Wait for a response from scheduler so that we know when it is finished
//	//with the message data pointer we sent it. Without this msgq_receive,
//	//this function will return and the pointer will contain garbage values when
//	//the scheduler tries to access it
//	_msgq_receive(qid, 0);
//
//
//	if(!_msgq_close(qid)) {
//		printf("\nCould not close message queue\n");
//		return 1;
//	}

	return MQX_OK;
}

uint32_t dd_return_list(TASK_LIST *list, int type) {

	if (type != ACTIVE_TASK_REQUEST_MESSAGE_TYPE || type != OVERDUE_TASK_REQUEST_MESSAGE_TYPE) {
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
		_task_block();
	}

	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, SCHEDULER_QUEUE);
	msg_ptr->HEADER.SOURCE_QID = qid;
	msg_ptr->HEADER.SIZE = sizeof(GENERIC_MESSAGE_PTR) + sizeof(int) + sizeof(void*);

	msg_ptr->TYPE = type;
	msg_ptr->DATA_PTR = NULL;


	bool result = _msgq_send(msg_ptr);
	if (result != TRUE) {
		printf("\nCould not send a message\n");
		return 1;
	}

	msg_ptr = _msgq_receive(qid, 0);
	if(msg_ptr != NULL) {
		TASK_LIST_DATA_PTR task_list_ptr = (TASK_LIST_DATA_PTR) msg_ptr->DATA_PTR;
		*list = task_list_ptr->TASK_LIST;
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

uint32_t dd_return_active_list(ACTIVE_TASK_LIST *list) {
	return dd_return_list(list, ACTIVE_TASK_REQUEST_MESSAGE_TYPE);
}

uint32_t dd_return_overdue_list(OVERDUE_TASK_LIST *list) {
	return dd_return_list(list, OVERDUE_TASK_REQUEST_MESSAGE_TYPE);
}

bool OpenR(uint16_t stream_no) {

	if(_mutex_lock(&readPrivilegeMutex) != MQX_EOK) {
		printf("Failed to lock the read privileges\n");
		return false;
	}

	_task_id id = _task_get_id();
	bool hasReadPermission = false;

	//Check if the current task already has read privileges
	int i;
	for(i = 0; i < MAX_TASKS_WITH_READ_PERM; ++i) {
		if(readPrivilege[i].task_id == id) {
			hasReadPermission = true;
			stream_no = readPrivilege[i].qid;
			break;
		}
	}

	if(hasReadPermission) {
		//User task already has read permission
		printf("User task already has read privileges\n");
		_mutex_unlock(&readPrivilegeMutex);
		return false;
	}

	//The current task does not already have read privileges,
	//so give it read privileges
	for(i = 0; i < MAX_TASKS_WITH_READ_PERM; ++i) {
		if(readPrivilege[i].task_id == MQX_NULL_TASK_ID) {
			readPrivilege[i].task_id = id;
			readPrivilege[i].qid = stream_no;
			hasReadPermission = true;
			stream_no = readPrivilege[i].qid;
			_mutex_unlock(&readPrivilegeMutex);
			return true;
		}
	}

	printf("Not enough space in the read permissions to add a new user task\n");
	_mutex_unlock(&readPrivilegeMutex);
	return false;
}

bool _getline(char *line) {
	if(_mutex_lock(&readPrivilegeMutex) != MQX_EOK) {
		printf("Failed to lock the read privileges\n");
		return false;
	}

	_task_id id = _task_get_id();
	bool hasReadPermission = false;
	_queue_id stream_no;

	//Check if the current task has read privileges
	int i;
	for(i = 0; i < MAX_TASKS_WITH_READ_PERM; ++i) {
		if(readPrivilege[i].task_id == id) {
			hasReadPermission = true;
			stream_no = readPrivilege[i].qid;
			break;
		}
	}

	if(!hasReadPermission) {
		//User task already has read permission
		printf("User task does not have read privileges\n");
		_mutex_unlock(&readPrivilegeMutex);
		return false;
	}

	_mutex_unlock(&readPrivilegeMutex);

	STRING_MESSAGE_PTR msg_ptr = _msgq_receive(stream_no, 0);
	if(msg_ptr != NULL && msg_ptr->TYPE == STRING_MESSAGE_TYPE) {
		strcpy(line, msg_ptr->DATA);
		_msg_free(msg_ptr);
		return true;
	}
	return false;
}

_queue_id OpenW(void) {

	if(_mutex_lock(&writePrivilegeMutex) != MQX_EOK) {
		printf("Failed to lock the write privileges\n");
		return MSGQ_NULL_QUEUE_ID;
	}

	if(writePrivilege.task_id != MQX_NULL_TASK_ID) {
		printf("Only 1 user task at a time can have write permission\n");
		_mutex_unlock(&writePrivilegeMutex);
		return MSGQ_NULL_QUEUE_ID;
	}

	//No task already has write privilege, so give the current task write privilege
	writePrivilege.task_id = _task_get_id();
	_queue_id qid = writePrivilege.qid;

	_mutex_unlock(&writePrivilegeMutex);
	return qid;
}


bool _putline(_queue_id qid, char *line) {
	if(_mutex_lock(&writePrivilegeMutex) != MQX_EOK) {
		printf("Failed to lock the write privileges\n");
		return false;
	}

	//If current task does not have write permission
	if(writePrivilege.task_id != _task_get_id()) {
		printf("Cannot write line because user task does not have write permission\n");
		_mutex_unlock(&writePrivilegeMutex);
		return false;
	}

	_mutex_unlock(&writePrivilegeMutex);

	//Allocate a string message
	STRING_MESSAGE_PTR msg_ptr = (STRING_MESSAGE_PTR) _msg_alloc(message_pool);
	if (msg_ptr == NULL){
		printf("\nCould not allocate a message\n");
		return false;
	}

	//Construct the message
	msg_ptr->TYPE = STRING_MESSAGE_TYPE;
	strcpy(msg_ptr->DATA, line);
	msg_ptr->HEADER.TARGET_QID = qid;
	msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(char)*BUFFER_LENGTH_WITH_NULL;

	//Send line to the handler
	bool result = _msgq_send(msg_ptr);
	if (result != TRUE) {
		printf("\nCould not send a message\n");
		return false;
	}

	return true;


}

bool Close(void) {
	_task_id id = _task_get_id();
	if(_mutex_lock(&writePrivilegeMutex) != MQX_EOK) {
		printf("Failed to lock the write privileges\n");
		return false;
	}

	if(_mutex_lock(&readPrivilegeMutex) != MQX_EOK) {
		printf("Failed to lock the read privileges\n");
		_mutex_unlock(&writePrivilegeMutex);
		return false;
	}

	bool success = false;

	//If the current task has write privilege, then remove that privilege
	if(writePrivilege.task_id == id) {
		writePrivilege.task_id = MQX_NULL_TASK_ID;
		success = true;
	}

	//Find the task id in the read privileges array and clear it if it exists
	int i;
	for(i = 0; i < MAX_TASKS_WITH_READ_PERM; ++i) {
		if(readPrivilege[i].task_id == id) {
			readPrivilege[i].task_id = MQX_NULL_TASK_ID;
			success = true;
			break;
		}

	}

	_mutex_unlock(&writePrivilegeMutex);
	_mutex_unlock(&readPrivilegeMutex);

	return success;
}

