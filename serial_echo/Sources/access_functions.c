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

bool _getline(unsigned char *line) {
	//TODO: Check if task has read permission
	//	return false if not

	GENERIC_MESSAGE_PTR msg_ptr = _msgq_receive(readPrivilege.stream_no, 0);
	if(msg_ptr != NULL && msg_ptr->BODY_PTR->TYPE == STRING_MESSAGE_TYPE) {
		unsigned char **ptr = (unsigned char **) msg_ptr->BODY_PTR->DATA;
		strcpy(line, *ptr);
		free(ptr);

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
	if(writePrivilege.task_id != 0) {
		printf("Only 1 user task at a time can have write permission\n");
		return 0;
	}
	writePrivilege.task_id = _task_get_id();
	return writePrivilege.qid;


}


bool _putline(_queue_id qid, unsigned char *line) {
	//TODO: We want a mutex here so that only 1 task
	//can access write_ptr at a time

	//If current task does not have write permission
	if(writePrivilege.task_id != _task_get_id()) {
		printf("Cannot write line because user task does not have write permission\n");
		return false;
	}


	//Allocate a string message
	GENERIC_MESSAGE_PTR msg_ptr = (GENERIC_MESSAGE_PTR) _msg_alloc(message_pool);
	if (msg_ptr == NULL){
		printf("\nCould not allocate a message\n");
		return false;
	}

//	unsigned char *str = malloc(sizeof(char) * (BUFFER_LENGTH+2));
//	sprintf(str, "%s\n", line);

	//Construct the message
	msg_ptr->BODY_PTR->TYPE = STRING_MESSAGE_TYPE;
	msg_ptr->BODY_PTR->DATA = &line;
	msg_ptr->HEADER.TARGET_QID = qid;
	msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(MESSAGE_BODY_PTR);

	//Send line to the handler
	bool result = _msgq_send(msg_ptr);
	if (result != TRUE) {
		printf("\nCould not send a message\n");
		return false;
	}

	//Wait for a response message from handler

	//if response indicates that there was an error
	//		return false


	return true;


}

bool Close(void) {

}

