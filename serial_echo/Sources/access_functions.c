/*
 * access_functions.c
 *
 *  Created on: Feb 6, 2017
 *      Author: lscholte
 */


#include "access_functions.h"
#include "os_tasks.h"

bool OpenR(uint16_t stream_no) {

}

bool _getline(char * line) {

}

_queue_id OpenW(void) {



	//TODO: We want a mutex here so that only 1 task
	//can access write_ptr at a time
	//if the task id is 0
	if(write_ptr->task_id == 0) {
		return 0;
	}
	write_ptr->task_id = _task_get_id();
	return write_ptr->qid;


}

bool _putline(_queue_id qid, char * line) {
	//TODO: We want a mutex here so that only 1 task
	//can access write_ptr at a time

	//If current task does not have write permission
	if(write_ptr->task_id != _task_get_id()) {
		return false;
	}


	//Allocate a string message
	STRING_MESSAGE_PTR msg_ptr;
	msg_ptr = (STRING_MESSAGE_PTR) _msg_alloc(message_pool);
	if (msg_ptr == NULL){
		printf("\nCould not allocate a message\n");
		_task_block();
	}

	//Construct the message
	msg_ptr->DATA = line;
	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, PUTLINE_QUEUE);
	msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + strlen((char *)msg_ptr->DATA) + 1;

	//Send character to the handler
	bool result = _msgq_send(msg_ptr);
	if (result != TRUE) {
		printf("\nCould not send a message\n");
		_task_block();
	}

	//Wait for a response message from handler

	//if response indicates that there was an error
	//		return false


	return true;


}

bool Close(void) {

}

