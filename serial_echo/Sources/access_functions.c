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

	//Lock read privilege structure


	if(readPrivilege.task_id == _task_get_id()) {
		//User task already has read permission
		return false;
	}

	readPrivilege.stream_no = stream_no;
	readPrivilege.task_id = _task_get_id();

	//for each item in read privileges
	//	if item.task_id == current task id
	//		return false
	//add current task to end of read privileges
	return true;
}

bool _getline(unsigned char * line) {
	//TODO: Check if task has read permission
	//	return false if not

	STRING_MESSAGE_PTR msg_ptr = _msgq_receive(readPrivilege.stream_no, 0);
	if(msg_ptr != NULL) {
		sprintf(line, msg_ptr->DATA);

		/* free the message */
		_msg_free(msg_ptr);
		return true;
	}
	return false;
}

_queue_id OpenW(void) {

	//TODO: We want a mutex here so that only 1 task
	//can access write_ptr at a time
	//if the task id is 0
	if(writePrivilege.task_id == 0) {
		return 0;
	}
	writePrivilege.task_id = _task_get_id();
	return writePrivilege.qid;


}


bool _putline(_queue_id qid, unsigned char * line) {
	//TODO: We want a mutex here so that only 1 task
	//can access write_ptr at a time

	//If current task does not have write permission
	if(writePrivilege.task_id != _task_get_id()) {
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

	//Send line to the handler
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

