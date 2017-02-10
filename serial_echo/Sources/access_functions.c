/*
 * access_functions.c
 *
 *  Created on: Feb 6, 2017
 *      Author: lscholte
 */


#include "access_functions.h"
#include "os_tasks.h"
#include "message_structs.h"

bool OpenR(uint16_t stream_no) {

	if(_mutex_lock(&readPrivilegeMutex) != MQX_EOK) {
		printf("Failed to lock the read privileges\n");
		return false;
	}

	//TODO: We want to be able to store multiple user tasks with read permissions.
	//Currently we can only store a single task

	if(readPrivilege.task_id == _task_get_id()) {
		//User task already has read permission
		printf("User task already has read privileges\n");
		_mutex_unlock(&readPrivilegeMutex);
		return false;
	}

	readPrivilege.stream_no = stream_no;
	readPrivilege.task_id = _task_get_id();

	_mutex_unlock(&readPrivilegeMutex);
	return true;
}

bool _getline(unsigned char *line) {
	if(_mutex_lock(&readPrivilegeMutex) != MQX_EOK) {
		printf("Failed to lock the read privileges\n");
		return false;
	}

	//TODO: Read privilege structure needs to be able to handle multiple tasks
	if(readPrivilege.task_id != _task_get_id()) {
		//User task already has read permission
		printf("User task does not have read privileges\n");
		_mutex_unlock(&readPrivilegeMutex);
		return false;
	}

	_mutex_unlock(&readPrivilegeMutex);

	GENERIC_MESSAGE_PTR msg_ptr = _msgq_receive(readPrivilege.stream_no, 0);
	if(msg_ptr != NULL && msg_ptr->BODY.TYPE == STRING_MESSAGE_TYPE) {
		unsigned char **ptr = (unsigned char **) msg_ptr->BODY.DATA;
		strcpy(line, *ptr);
		free(ptr);

		/* free the message */
		_msg_free(msg_ptr);
		return true;
	}
	return false;
}

_queue_id OpenW(void) {

	if(_mutex_lock(&writePrivilegeMutex) != MQX_EOK) {
		printf("Failed to lock the write privileges\n");
		return 0;
	}

	if(writePrivilege.task_id != 0) {
		printf("Only 1 user task at a time can have write permission\n");
		_mutex_unlock(&writePrivilegeMutex);
		return 0;
	}
	writePrivilege.task_id = _task_get_id();
	_queue_id qid = writePrivilege.qid;

	_mutex_unlock(&writePrivilegeMutex);
	return qid;
}


bool _putline(_queue_id qid, unsigned char *line) {
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
	GENERIC_MESSAGE_PTR msg_ptr = (GENERIC_MESSAGE_PTR) _msg_alloc(message_pool);
	if (msg_ptr == NULL){
		printf("\nCould not allocate a message\n");
		return false;
	}

	//Construct the message
	msg_ptr->BODY.TYPE = STRING_MESSAGE_TYPE;
	msg_ptr->BODY.DATA = &line;
	msg_ptr->HEADER.TARGET_QID = qid;
	msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(MESSAGE_BODY);

	//Send line to the handler
	bool result = _msgq_send(msg_ptr);
	if (result != TRUE) {
		printf("\nCould not send a message\n");
		return false;
	}

	return true;


}

bool Close(void) {
	//TODO
}

