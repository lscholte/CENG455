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

_task_id dd_create(uint32_t template_index, uint32_t deadline) {
	return 0;
}

uint32_t dd_delete(_task_id task_id) {
	return 0;
}

uint32_t dd_return_active_list(ACTIVE_TASK_LIST *list) {
	return 0;
}

uint32_t dd_return_overdue_list(OVERDUE_TASK_LIST *list) {
	return 0;
}

//bool OpenR(uint16_t stream_no) {
//
//	if(_mutex_lock(&readPrivilegeMutex) != MQX_EOK) {
//		printf("Failed to lock the read privileges\n");
//		return false;
//	}
//
//	_task_id id = _task_get_id();
//	bool hasReadPermission = false;
//
//	//Check if the current task already has read privileges
//	int i;
//	for(i = 0; i < MAX_TASKS_WITH_READ_PERM; ++i) {
//		if(readPrivilege[i].task_id == id) {
//			hasReadPermission = true;
//			stream_no = readPrivilege[i].qid;
//			break;
//		}
//	}
//
//	if(hasReadPermission) {
//		//User task already has read permission
//		printf("User task already has read privileges\n");
//		_mutex_unlock(&readPrivilegeMutex);
//		return false;
//	}
//
//	//The current task does not already have read privileges,
//	//so give it read privileges
//	for(i = 0; i < MAX_TASKS_WITH_READ_PERM; ++i) {
//		if(readPrivilege[i].task_id == MQX_NULL_TASK_ID) {
//			readPrivilege[i].task_id = id;
//			readPrivilege[i].qid = stream_no;
//			hasReadPermission = true;
//			stream_no = readPrivilege[i].qid;
//			_mutex_unlock(&readPrivilegeMutex);
//			return true;
//		}
//	}
//
//	printf("Not enough space in the read permissions to add a new user task\n");
//	_mutex_unlock(&readPrivilegeMutex);
//	return false;
//}
//
//bool _getline(char *line) {
//	if(_mutex_lock(&readPrivilegeMutex) != MQX_EOK) {
//		printf("Failed to lock the read privileges\n");
//		return false;
//	}
//
//	_task_id id = _task_get_id();
//	bool hasReadPermission = false;
//	_queue_id stream_no;
//
//	//Check if the current task has read privileges
//	int i;
//	for(i = 0; i < MAX_TASKS_WITH_READ_PERM; ++i) {
//		if(readPrivilege[i].task_id == id) {
//			hasReadPermission = true;
//			stream_no = readPrivilege[i].qid;
//			break;
//		}
//	}
//
//	if(!hasReadPermission) {
//		//User task already has read permission
//		printf("User task does not have read privileges\n");
//		_mutex_unlock(&readPrivilegeMutex);
//		return false;
//	}
//
//	_mutex_unlock(&readPrivilegeMutex);
//
//	STRING_MESSAGE_PTR msg_ptr = _msgq_receive(stream_no, 0);
//	if(msg_ptr != NULL && msg_ptr->TYPE == STRING_MESSAGE_TYPE) {
//		strcpy(line, msg_ptr->DATA);
//		_msg_free(msg_ptr);
//		return true;
//	}
//	return false;
//}
//
//_queue_id OpenW(void) {
//
//	if(_mutex_lock(&writePrivilegeMutex) != MQX_EOK) {
//		printf("Failed to lock the write privileges\n");
//		return MSGQ_NULL_QUEUE_ID;
//	}
//
//	if(writePrivilege.task_id != MQX_NULL_TASK_ID) {
//		printf("Only 1 user task at a time can have write permission\n");
//		_mutex_unlock(&writePrivilegeMutex);
//		return MSGQ_NULL_QUEUE_ID;
//	}
//
//	//No task already has write privilege, so give the current task write privilege
//	writePrivilege.task_id = _task_get_id();
//	_queue_id qid = writePrivilege.qid;
//
//	_mutex_unlock(&writePrivilegeMutex);
//	return qid;
//}
//
//
//bool _putline(_queue_id qid, char *line) {
//	if(_mutex_lock(&writePrivilegeMutex) != MQX_EOK) {
//		printf("Failed to lock the write privileges\n");
//		return false;
//	}
//
//	//If current task does not have write permission
//	if(writePrivilege.task_id != _task_get_id()) {
//		printf("Cannot write line because user task does not have write permission\n");
//		_mutex_unlock(&writePrivilegeMutex);
//		return false;
//	}
//
//	_mutex_unlock(&writePrivilegeMutex);
//
//	//Allocate a string message
//	STRING_MESSAGE_PTR msg_ptr = (STRING_MESSAGE_PTR) _msg_alloc(message_pool);
//	if (msg_ptr == NULL){
//		printf("\nCould not allocate a message\n");
//		return false;
//	}
//
//	//Construct the message
//	msg_ptr->TYPE = STRING_MESSAGE_TYPE;
//	strcpy(msg_ptr->DATA, line);
//	msg_ptr->HEADER.TARGET_QID = qid;
//	msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(char)*BUFFER_LENGTH_WITH_NULL;
//
//	//Send line to the handler
//	bool result = _msgq_send(msg_ptr);
//	if (result != TRUE) {
//		printf("\nCould not send a message\n");
//		return false;
//	}
//
//	return true;
//
//
//}
//
//bool Close(void) {
//	_task_id id = _task_get_id();
//	if(_mutex_lock(&writePrivilegeMutex) != MQX_EOK) {
//		printf("Failed to lock the write privileges\n");
//		return false;
//	}
//
//	if(_mutex_lock(&readPrivilegeMutex) != MQX_EOK) {
//		printf("Failed to lock the read privileges\n");
//		_mutex_unlock(&writePrivilegeMutex);
//		return false;
//	}
//
//	bool success = false;
//
//	//If the current task has write privilege, then remove that privilege
//	if(writePrivilege.task_id == id) {
//		writePrivilege.task_id = MQX_NULL_TASK_ID;
//		success = true;
//	}
//
//	//Find the task id in the read privileges array and clear it if it exists
//	int i;
//	for(i = 0; i < MAX_TASKS_WITH_READ_PERM; ++i) {
//		if(readPrivilege[i].task_id == id) {
//			readPrivilege[i].task_id = MQX_NULL_TASK_ID;
//			success = true;
//			break;
//		}
//
//	}
//
//	_mutex_unlock(&writePrivilegeMutex);
//	_mutex_unlock(&readPrivilegeMutex);
//
//	return success;
//}

