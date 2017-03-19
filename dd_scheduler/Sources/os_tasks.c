/* ###################################################################
 **     Filename    : os_tasks.c
 **     Project     : serial_echo
 **     Processor   : MK64FN1M0VLL12
 **     Component   : Events
 **     Version     : Driver 01.00
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2017-02-05, 13:47, # CodeGen: 1
 **     Abstract    :
 **         This is user's event module.
 **         Put your event handler code here.
 **     Settings    :
 **     Contents    :
 **         handler_task - void handler_task(os_task_param_t task_init_data);
 **
 ** ###################################################################*/
/*!
 ** @file os_tasks.c
 ** @version 01.00
 ** @brief
 **         This is user's event module.
 **         Put your event handler code here.
 */
/*!
 **  @addtogroup os_tasks_module os_tasks module documentation
 **  @{
 */
/* MODULE os_tasks */


#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"
#include <stdio.h>
#include "message_structs.h"
#include "access_functions.h"
#include <timer.h>


#ifdef __cplusplus
extern "C" {
#endif 

_pool_id message_pool;
WRITE_PRIVILEGE writePrivilege;
READ_PRIVILEGE readPrivilege[MAX_TASKS_WITH_READ_PERM];

void printBackspaceToTerminal() {
	char sequence[3] = { '\b', ' ', '\b' };
	UART_DRV_SendDataBlocking(myUART_IDX, sequence, sizeof(sequence), 1000);
}

void printBackspaceToBuffer(char buffer[]) {
	int lastCharPosition = strlen(buffer) - 1;
	buffer[lastCharPosition] = 0;
}

// returns the index of the first non-space character in the buffer
int removeTrailingSpacesFromTerminal(char buffer[]) {
	int charPos = strlen(buffer) - 1;
	while(charPos >= 0 && buffer[charPos] == ' ') {
		printBackspaceToTerminal();
		charPos--;
	}
	return charPos;
}

void removeTrailingSpacesFromBuffer(char buffer[]) {
	int charPos = strlen(buffer) - 1;
	while(charPos >= 0 && buffer[charPos] == ' ') {
		printBackspaceToBuffer(buffer);
		charPos--;
	}
}

void printDeleteWordToTerminal(char buffer[]) {
	int lastCharPosition = removeTrailingSpacesFromTerminal(buffer);
	int i;
	for(i = lastCharPosition; i >= 0; --i) {
		if(buffer[i] == ' ') {
			printBackspaceToTerminal();
			break;
		}
		printBackspaceToTerminal();
	}
}

void printDeleteWordToBuffer(char buffer[]) {
	removeTrailingSpacesFromBuffer(buffer);
	int lastCharPosition = strlen(buffer) - 1;
	int i;
	for(i = lastCharPosition; i >= 0; --i) {
		if(buffer[i] == ' ') {
			printBackspaceToBuffer(buffer);
			break;
		}
		printBackspaceToBuffer(buffer);
	}
}

void printDeleteLineToTerminal(char buffer[]) {
	int lastCharPosition = strlen(buffer) - 1;
	int i;
	for(i = lastCharPosition; i >= 0; --i) {
		printBackspaceToTerminal();
	}
}

void printDeleteLineToBuffer(char buffer[]) {
	memset(buffer, 0, BUFFER_LENGTH);
}

void printNewlineToTerminal() {
	char sequence[2] = { '\n', '\r' };
	UART_DRV_SendDataBlocking(myUART_IDX, sequence, sizeof(sequence), 1000);
}

void printCharacterToTerminal(char c) {
	UART_DRV_SendDataBlocking(myUART_IDX, &c, sizeof(char), 1000);
}

bool printCharacterToBuffer(char c, char buffer[]) {
	int newCharPosition = strlen(buffer);
	if(newCharPosition >= BUFFER_LENGTH) {
		printf("Maximum line length reached. Cannot print character %c\n", c);
		return false;
	}
	buffer[newCharPosition] = c;
	return true;
}

void handleCharacter(char c, char *buffer) {
	if(_mutex_lock(&readPrivilegeMutex) != MQX_EOK) {
		printf("Failed to lock the read privileges\n");
		return;
	}

	bool taskWithReadPermission = false;
	int i;
	for(i = 0; i < MAX_TASKS_WITH_READ_PERM; ++i) {
		if(readPrivilege[i].task_id != MQX_NULL_TASK_ID) {
			taskWithReadPermission = true;
			break;
		}

	}

	if(!taskWithReadPermission) {
		printf("No user tasks with read privileges. Discarding character\n");
		_mutex_unlock(&readPrivilegeMutex);
		return;
	}

	STRING_MESSAGE_PTR msg_ptr;
	switch(c) {
	case 0x08: //backspace
		printBackspaceToTerminal();
		printBackspaceToBuffer(buffer);
		break;
	case 0x17: //delete word
		printDeleteWordToTerminal(buffer);
		printDeleteWordToBuffer(buffer);
		break;
	case 0x15: //delete line
		printDeleteLineToTerminal(buffer);
		printDeleteLineToBuffer(buffer);
		break;
	case '\n':
	case '\r':
		//Allocate a string message
		for(i = 0; i < MAX_TASKS_WITH_READ_PERM; ++i) {
			if(readPrivilege[i].task_id != MQX_NULL_TASK_ID) {
				msg_ptr = (STRING_MESSAGE_PTR) _msg_alloc(message_pool);
				if (msg_ptr == NULL){
					printf("\nCould not allocate a message\n");
					return;
				}

				//Construct the message
				msg_ptr->TYPE = STRING_MESSAGE_TYPE;
				strcpy(msg_ptr->DATA, buffer);
				msg_ptr->HEADER.TARGET_QID = readPrivilege[i].qid;
				msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(char)*BUFFER_LENGTH_WITH_NULL;

				//Send line to the handler
				bool result = _msgq_send(msg_ptr);
				if (result != TRUE) {
					printf("\nCould not send a message\n");
					return;
				}
			}
		}

		printNewlineToTerminal();
		printDeleteLineToBuffer(buffer);
		break;

	default: //printable character
		if(printCharacterToBuffer(c, buffer)) {
			printCharacterToTerminal(c);
		}
		break;
	}
	_mutex_unlock(&readPrivilegeMutex);
}

void handleString(char *string, char buffer[]) {
	printDeleteLineToTerminal(buffer);
	UART_DRV_SendDataBlocking(myUART_IDX, string, strlen(string), 1000);
	char sequence[2] = { '\n', '\r' };
	UART_DRV_SendDataBlocking(myUART_IDX, sequence, sizeof(sequence), 1000);
	UART_DRV_SendDataBlocking(myUART_IDX, buffer, BUFFER_LENGTH, 1000);
}

uint32_t getCurrentTime() {
	TIME_STRUCT time_struct;
	_time_get(&time_struct);
	return time_struct.MILLISECONDS + time_struct.SECONDS * 1000;
}

void doDelete(_task_id tid, ACTIVE_TASK_LIST *active_tasks, TASK_NODE **running_task_node_ptr) {
	if(_task_destroy(tid) != MQX_OK) {
//		printf("\nCould not destroy task. Maybe task has already finished\n");
	}

	//Find the task to remove in the priority queue
	TASK_NODE *task_node_ptr = NULL;
	for(task_node_ptr = active_tasks->head;
			task_node_ptr != NULL;
			task_node_ptr = task_node_ptr->next_node) {
		if(task_node_ptr->tid == tid) {
			break;
		}
	}

	remove(active_tasks, task_node_ptr);
	task_node_ptr = NULL;

	//If the deleted task is the one currently running,
	//	then we need to reschedule. Otherwise, the current
	//	task can just keep running
	if((*running_task_node_ptr)->tid == tid) {

		//We need to reschedule by finding
		//getting the task with the soonest deadline
		//from the priority queue.
		TASK_NODE *soonest_task_node_ptr = NULL;
		task_node_ptr = NULL;
		for(task_node_ptr = active_tasks->head;
				task_node_ptr != NULL;
				task_node_ptr = task_node_ptr->next_node) {
			if(soonest_task_node_ptr == NULL ||
					task_node_ptr->absolute_deadline < soonest_task_node_ptr->absolute_deadline) {
				soonest_task_node_ptr = task_node_ptr;
			}
		}

		//Set new active task's priority to RUNNING_TASK_PRIORITY.
		_mqx_uint temp;
		_task_set_priority(soonest_task_node_ptr->tid, RUNNING_TASK_PRIORITY, &temp);


		//At this point task has already been deleted, so I don't think
		//think we need to adjust the running task's priority. In fact,
		//it might even be undefined behaviour if we tried to do that

		//Set running_task_node_ptr to that new active task
		*running_task_node_ptr = soonest_task_node_ptr;
	}
}



/* User includes (#include below this line is not maintained by Processor Expert) */

/*
 ** ===================================================================
 **     Callback    : handler_task
 **     Description : Task function entry.
 **     Parameters  :
 **       task_init_data - OS task parameter
 **     Returns : Nothing
 ** ===================================================================
 */
void handler_task(os_task_param_t task_init_data)
{
	printf("Handler Task Created!\n");

	//create a task list here
	//now we have a pointer to that list
	//dd_return_active_list(list);

	//Open the handler message queue
	_queue_id handler_qid = _msgq_open(HANDLER_QUEUE, 0);

	if(_mutex_lock(&writePrivilegeMutex) != MQX_EOK) {
		printf("Failed to lock the write privileges\n");
		_task_block();
	}

	//Init the global write privileges struct
	writePrivilege.qid = handler_qid;
	writePrivilege.task_id = MQX_NULL_TASK_ID;

	_mutex_unlock(&writePrivilegeMutex);

	if(_mutex_lock(&readPrivilegeMutex) != MQX_EOK) {
		printf("Failed to lock the read privileges\n");
		_task_block();
	}

	//Init the global read privileges struct
	int i;
	for(i = 0; i < MAX_TASKS_WITH_READ_PERM; ++i) {
		readPrivilege[i].task_id = MQX_NULL_TASK_ID;
		readPrivilege[i].qid = MSGQ_NULL_QUEUE_ID;
	}

	_mutex_unlock(&readPrivilegeMutex);

	if (handler_qid == MSGQ_NULL_QUEUE_ID) {
		printf("Could not open the server message queue\n");
		_task_block();
	}

	message_pool = _msgpool_create(sizeof(STRING_MESSAGE), NUM_CLIENTS, 1, 0);

	if (message_pool == MSGPOOL_NULL_POOL_ID) {
		printf("Count not create a message pool\n");
		_task_block();
	}

	char buffer[BUFFER_LENGTH_WITH_NULL] = { 0 };

#ifdef PEX_USE_RTOS
	while (1) {
#endif
		//Wait for a message
		STRING_MESSAGE_PTR msg_ptr = _msgq_receive(handler_qid, 0);
		if(msg_ptr != NULL) {

			//If this is a character message, then it is from ISR
			if(msg_ptr->TYPE == CHAR_MESSAGE_TYPE) {
				char c =  msg_ptr->DATA[0];
				_msg_free(msg_ptr);
				handleCharacter(c, buffer);
			}

			//If this is a string message, then it is from putline
			else if(msg_ptr->TYPE == STRING_MESSAGE_TYPE) {
				char *line = msg_ptr->DATA;
				_msg_free(msg_ptr);
				handleString(line, buffer);
			}


		}


#ifdef PEX_USE_RTOS   
	}
#endif    
}

/*
 ** ===================================================================
 **     Callback    : generator_task
 **     Description : Task function entry.
 **     Parameters  :
 **       task_init_data - OS task parameter
 **     Returns : Nothing
 ** ===================================================================
 */
//void generator_task(os_task_param_t task_init_data)
//{
//	printf("Master Task Created!\n");
//
//	_queue_id generator_task_qid = _msgq_open(MSGQ_FREE_QUEUE, 0);
//	OpenR(generator_task_qid);
//	OpenW();
//	char line[BUFFER_LENGTH_WITH_NULL];
//
//#ifdef PEX_USE_RTOS
//  while (1) {
//#endif
//
//	if(_getline(line)) {
//		printf("Master Task Line Received: %s\n", line);
//		if(strlen(line) > 0 ){
//			switch(line[0]){
//				case '~':
//					_task_create(0, SLAVETASK_TASK, (uint32_t)(NULL));
//					break;
//				case '*':
//					if(strlen(line) > 1) {
//						char *pre = "Master Task says: ";
//						int dest_size = strlen(pre) + strlen(&line[1]);
//						char dest[dest_size];
//						strcpy(dest, pre);
//						strcat(dest, &line[1]);
//						if(_putline(_msgq_get_id(0, HANDLER_QUEUE), dest)) {
//							printf("Master Task Line Sent: %s\n", &line[1]);
//						}
//					}
//					break;
//				case '.':
//					if(Close()) {
//						printf("Master Task Closed\n");
//					}
//					break;
//			}
//		}
//	} else {
//		_task_block();
//	}
//
//#ifdef PEX_USE_RTOS
//  }
//#endif
//}

/*
 ** ===================================================================
 **     Callback    : slave_task
 **     Description : Task function entry.
 **     Parameters  :
 **       task_init_data - OS task parameter
 **     Returns : Nothing
 ** ===================================================================
 */
void slave_task(os_task_param_t task_init_data)
{
	printf("Slave task created with runtime %u\n", task_init_data);
    synthetic_compute(task_init_data); // task's actual computation simulated by a busy loop
//    dd_delete (_task_get_id ());

//	_task_id id = _task_get_id();
//	printf("Slave Task %u Created!\n", id);
//
//	_queue_id generator_task_qid = _msgq_open(MSGQ_FREE_QUEUE, 0);
//
//	OpenR(generator_task_qid);
//	char line[BUFFER_LENGTH_WITH_NULL];
//
//#ifdef PEX_USE_RTOS
//	while (1) {
//#endif
//
//		if(_getline(line)) {
//			printf("Slave %d Received: %s\n", id, line);
//
//			if(strlen(line) > 0 ) {
//
//				char firstFive[6];
//				strncpy(firstFive, line, 5);
//				firstFive[5]= '\0';
//				char idStr[5];
//				sprintf(idStr, "%d", id);
//
//				if(strcmp(idStr, firstFive) == 0) {
//					if(line[5] =='*') {
//						char pre[20];
//						sprintf(pre, "Slave %d says: ", id);
//						int dest_size = strlen(pre) + strlen(&line[6]);
//						char dest[dest_size];
//						strcpy(dest, pre);
//						strcat(dest, &line[6]);
//						if(_putline(_msgq_get_id(0, HANDLER_QUEUE), dest)) {
//							printf("Slave %d Line Sent: %s\n", id, &line[6]);
//						}
//					}
//					else if(line[5] =='!') {
//						if(OpenW()) {
//							printf("Slave %d Granted write permission\n", id);
//						}
//					}
//					else if(line[5] =='.') {
//						if(Close()) {
//							printf("Slave %d Closed\n", id);
//						}
//					}
//				}
//			}
//		} else {
//			_task_block();
//		}
//
//
//#ifdef PEX_USE_RTOS
//	}
//#endif
}

/*
 ** ===================================================================
 **     Callback    : dd_scheduler_task
 **     Description : Task function entry.
 **     Parameters  :
 **       task_init_data - OS task parameter
 **     Returns : Nothing
 ** ===================================================================
 */
void dd_scheduler_task(os_task_param_t task_init_data)
{
	printf("DD Scheduler task created\n");

	//Open the handler message queue
	_queue_id scheduler_qid = _msgq_open(SCHEDULER_QUEUE, 0);

	message_pool = _msgpool_create(sizeof(GENERIC_MESSAGE), NUM_CLIENTS, 1, 0);

	if (message_pool == MSGPOOL_NULL_POOL_ID) {
		printf("Count not create a message pool\n");
		_task_block();
	}

	_mqx_uint temp;

//	_task_id idle_tid = _task_create(0, IDLETASK_TASK, (uint32_t)(NULL));
//	_task_set_priority(idle_tid, IDLE_TASK_PRIORITY, &temp);

	_mqx_uint newPriority = PRIORITY_OSA_TO_RTOS(GENERATORTASK_TASK_PRIORITY + 1U);
	_task_set_priority(_task_get_id(), newPriority, &temp);


	//TODO: Setup an Idle task that has priority of IDLE_TASK_PRIORITY.
	//This task does nothing except sit in a while loop forever. When the running
	//task finishes executing, then the idle task becomes the task with highest
	//priority, so it will run until we schedule a new task to run

	//Init empty overdue task list
	OVERDUE_TASK_LIST overdue_tasks;
	overdue_tasks.head = NULL;

	//Setup a priority queue of tasks sorted by deadline
	//Then remove min from the priority queue, which will give us the task
	//with the soonest deadline. Set this task's priority to RUNNING_TASK_PRIORITY.
	//The priority queue could be a heap, sorted list, or unsorted list. For now,
	//we'll use an unsorted list since it is easiest to implement
	ACTIVE_TASK_LIST active_tasks;
	active_tasks.head = NULL;


	TASK_NODE *running_task_node_ptr = NULL;
	TASK_NODE *soonest_task_node_ptr = NULL;
	TASK_NODE *task_node_ptr = NULL;

#ifdef PEX_USE_RTOS
	while (1) {
#endif

		soonest_task_node_ptr = NULL;
		task_node_ptr = NULL;
		for(task_node_ptr = active_tasks.head;
				task_node_ptr != NULL;
				task_node_ptr = task_node_ptr->next_node) {
			if(soonest_task_node_ptr == NULL ||
					task_node_ptr->absolute_deadline < soonest_task_node_ptr->absolute_deadline) {
				soonest_task_node_ptr = task_node_ptr;
			}
		}

		uint32_t currentTimeMillis = getCurrentTime();
		_mqx_uint shortestRelativeDeadline = 0;

		GENERIC_MESSAGE_PTR msg_ptr = NULL;
		if(soonest_task_node_ptr != NULL) {
			if(currentTimeMillis < soonest_task_node_ptr->absolute_deadline) {
				shortestRelativeDeadline = soonest_task_node_ptr->absolute_deadline - currentTimeMillis;
				msg_ptr = _msgq_receive(scheduler_qid, shortestRelativeDeadline);
			}
		}
		else {
			msg_ptr = _msgq_receive(scheduler_qid, 0);
		}

		currentTimeMillis = getCurrentTime();

		if(msg_ptr != NULL) {
			if(msg_ptr->TYPE == TASK_CREATION_MESSAGE_TYPE) {
				TASK_CREATION_DATA_PTR data_ptr = (TASK_CREATION_DATA_PTR) msg_ptr->DATA_PTR;
				TASK_CREATION_DATA data = *data_ptr;

				_task_id tid = data.TASK_ID;
				uint32_t relativeDeadline = data.DEADLINE;

				uint32_t absoluteDeadline = currentTimeMillis + relativeDeadline;

				//Insert the new task into the priority queue. Since we have an
				//unsorted list, we're just adding to the front for simplicity
				TASK_NODE *task_node_ptr = (TASK_NODE *) malloc(sizeof(TASK_NODE));
				task_node_ptr->creation_time = 0; //TODO: set to current time
				task_node_ptr->absolute_deadline = absoluteDeadline;
				task_node_ptr->next_node = active_tasks.head;
				task_node_ptr->previous_node = NULL;
				task_node_ptr->task_type = 0; //TODO: set this to something.
				task_node_ptr->tid = tid;

				addToFront(&active_tasks, task_node_ptr);

				//This is simply because _task_set_priority wants us to pass in
				//a ptr to the old priority. We don't actually care about this,
				//but I don't think we are allowed to pass NULL
				_mqx_uint temp;

				//If new task deadline is less than currently running task deadline,
				//	then set new task deadline to RUNNING_TASK_PRIORITY
				//	and set old task deadline to WAITING_TASK_PRIORITY
				if(running_task_node_ptr == NULL ||
						absoluteDeadline < running_task_node_ptr->absolute_deadline) {


					//Set the priority of the new task to the highest priority
					_task_set_priority(tid, RUNNING_TASK_PRIORITY, &temp);

					//Lower the priority of the old task
					if(running_task_node_ptr != NULL) {
						_task_set_priority(running_task_node_ptr->tid, WAITING_TASK_PRIORITY, &temp);
					}

					running_task_node_ptr = task_node_ptr;
				}
				else {
					_task_set_priority(tid, WAITING_TASK_PRIORITY, &temp);
				}


				msg_ptr->HEADER.TARGET_QID = msg_ptr->HEADER.SOURCE_QID;
				msg_ptr->HEADER.SOURCE_QID = scheduler_qid;
				msg_ptr->DATA_PTR = NULL;

				bool result = _msgq_send(msg_ptr);
				if (result != TRUE) {
					printf("\nCould not send a message\n");
					_task_block();
				}

			}

			if(msg_ptr->TYPE == TASK_DELETION_MESSAGE_TYPE) {
				TASK_DELETION_DATA_PTR data_ptr = (TASK_DELETION_DATA_PTR) msg_ptr->DATA_PTR;
				TASK_DELETION_DATA data = *data_ptr;

				_task_id tid = data.TASK_ID;

				free(data_ptr);
				_msg_free(msg_ptr);

				doDelete(tid, &active_tasks, &running_task_node_ptr);
			}

			if(msg_ptr->TYPE == ACTIVE_TASK_REQUEST_MESSAGE_TYPE) {

				msg_ptr->HEADER.TARGET_QID = msg_ptr->HEADER.SOURCE_QID;
				msg_ptr->HEADER.SOURCE_QID = scheduler_qid;

				//TODO: We really should send a copy of the active task list
				msg_ptr->DATA_PTR = &active_tasks;

				bool result = _msgq_send(msg_ptr);
				if (result != TRUE) {
					printf("\nCould not send active task response message\n");
					_task_block();
				}
			}

			if(msg_ptr->TYPE == OVERDUE_TASK_REQUEST_MESSAGE_TYPE) {

				msg_ptr->HEADER.TARGET_QID = msg_ptr->HEADER.SOURCE_QID;
				msg_ptr->HEADER.SOURCE_QID = scheduler_qid;

				//TODO: We really should send a copy of the active task list
				msg_ptr->DATA_PTR = &overdue_tasks;

				bool result = _msgq_send(msg_ptr);
				if (result != TRUE) {
					printf("\nCould not send overdue task response message\n");
					_task_block();
				}
			}

		}
		else {
			//msgq_receive timed out

			//Find all tasks in the active task list that have gone past their deadline
			TASK_NODE *task_node_ptr = NULL;
			for(task_node_ptr = active_tasks.head;
					task_node_ptr != NULL;
					task_node_ptr = task_node_ptr->next_node) {
				if(task_node_ptr->absolute_deadline <= currentTimeMillis) {

//					printf("Found overdue task: %u\n", task_node_ptr->tid);

					//Create copy of the active task node to be inserted in the overdue task list
					TASK_NODE *overdue_task_node_ptr = (TASK_NODE *) malloc(sizeof(TASK_NODE));
					overdue_task_node_ptr->creation_time = task_node_ptr->creation_time;
					overdue_task_node_ptr->absolute_deadline = task_node_ptr->absolute_deadline;
					overdue_task_node_ptr->next_node = task_node_ptr->next_node;
					overdue_task_node_ptr->previous_node = task_node_ptr->previous_node;
					overdue_task_node_ptr->task_type = task_node_ptr->task_type;
					overdue_task_node_ptr->tid = task_node_ptr->tid;

					//Task is overdue, so delete the task from the scheduler
					doDelete(task_node_ptr->tid, &active_tasks, &running_task_node_ptr);

					//Add task to overdue task list
					addToFront(&overdue_tasks, overdue_task_node_ptr);

				}
			}
		}

#ifdef PEX_USE_RTOS   
	}
#endif    
}

// IS CALLED WHEN TIMER EXPIRES
void timer_callback(_timer_id t, void* dataptr, unsigned int seconds, unsigned int miliseconds){
    (*(bool*)dataptr) = false;
}

// CREATE BUSY-WAIT DELAY FOR A GIVEN DURATION
void synthetic_compute(unsigned int mseconds){
    bool flag = true;
	unsigned int miliseconds = mseconds;
	_timer_start_oneshot_after(timer_callback, &flag, TIMER_KERNEL_TIME_MODE, miliseconds);

    // busy wait loop
    while (flag){}
}

/*
 ** ===================================================================
 **     Callback    : generator_task
 **     Description : Task function entry.
 **     Parameters  :
 **       task_init_data - OS task parameter
 **     Returns : Nothing
 ** ===================================================================
 */
void generator_task(os_task_param_t task_init_data)
{
//	printf("Generator Task created\n");
//    ACTIVE_TASK_LIST active_tasks;
//    OVERDUE_TASK_LIST overdue_tasks;
//
//    // CREATE SAMPLE TASKS
//    int n_total_tasks = 3;
//    dd_tcreate(SLAVETASK_TASK, 500);
//    _time_delay(1000);
//    dd_tcreate(SLAVETASK_TASK, 1500);
//    dd_tcreate(SLAVETASK_TASK, 3000);
//
//    printf("TASK GENERATOR: %d tasks created.\n\r", n_total_tasks);
//
//    // WAIT FOR CERTAIN TIME
//    _time_delay(3000);
//
//    // OBTAIN STATUS FROM SCHEDULER
//    printf("TASK GENERATOR: collecting statistics\n\r");
//
//    if(dd_return_overdue_list(&overdue_tasks) != MQX_OK) {
//        printf("error: failed to obtain the overdue task list!\n\r");
//        return;
//    }
//
//    if(dd_return_active_list(&active_tasks) != MQX_OK) {
//        printf("error: failed to obtain the active task list!\n\r");
//        return;
//    }
//
//    int n_completed_tasks = 0;
//    int n_failed_tasks = 0;
//    int n_running_tasks = 0;
//
//    TASK_NODE *temp_at_ptr = active_tasks.head;
//    while(temp_at_ptr){
//        n_running_tasks++;
//        temp_at_ptr = temp_at_ptr->next_node;
//    }
//
//    TASK_NODE *temp_ot_ptr = overdue_tasks.head;
//    while(temp_ot_ptr){
//        n_failed_tasks++;
//        temp_ot_ptr = temp_ot_ptr->next_node;
//    }
//
//    n_completed_tasks = n_total_tasks-(n_failed_tasks+n_running_tasks);
//    printf("TASK GENERATOR: %d failed, %d completed, %d still running.\n\r", n_failed_tasks, n_completed_tasks, n_running_tasks);

//    return;



	printf("Generator Task Created!\n");

	_queue_id generator_task_qid = _msgq_open(MSGQ_FREE_QUEUE, 0);
	OpenR(generator_task_qid);
	OpenW();
	char line[BUFFER_LENGTH_WITH_NULL];

    int n_total_tasks = 0;

#ifdef PEX_USE_RTOS
	while (1) {
#endif

		if(_getline(line)) {
//			printf("Master Task Line Received: %s\n", line);
			if(strlen(line) > 0 ){
				switch(line[0]){
				case '~':
//					_task_create(0, SLAVETASK_TASK, (uint32_t)(NULL));
					if(strlen(line) > 1) {
						int dest_size = strlen(&line[1]);
						char dest[dest_size];
						strcpy(dest, &line[1]);
						dd_tcreate(SLAVETASK_TASK, atoi(dest), 2000);
					}
					else {
						dd_tcreate(SLAVETASK_TASK, 3000, 1000);
					}
					++n_total_tasks;
					break;
				case '*':
//					if(strlen(line) > 1) {
//						char *pre = "Master Task says: ";
//						int dest_size = strlen(pre) + strlen(&line[1]);
//						char dest[dest_size];
//						strcpy(dest, pre);
//						strcat(dest, &line[1]);
//						if(_putline(_msgq_get_id(0, HANDLER_QUEUE), dest)) {
//							printf("Master Task Line Sent: %s\n", &line[1]);
//						}
//					}
				    // OBTAIN STATUS FROM SCHEDULER
				    printf("TASK GENERATOR: collecting statistics\n\r");

				    ACTIVE_TASK_LIST active_tasks;
				    OVERDUE_TASK_LIST overdue_tasks;

				    if(dd_return_overdue_list(&overdue_tasks) != MQX_OK) {
				        printf("error: failed to obtain the overdue task list!\n\r");
				        return;
				    }

				    if(dd_return_active_list(&active_tasks) != MQX_OK) {
				        printf("error: failed to obtain the active task list!\n\r");
				        return;
				    }

				    int n_completed_tasks = 0;
				    int n_failed_tasks = 0;
				    int n_running_tasks = 0;

				    TASK_NODE *temp_at_ptr = active_tasks.head;
				    while(temp_at_ptr){
				        n_running_tasks++;
				        temp_at_ptr = temp_at_ptr->next_node;
				    }

				    TASK_NODE *temp_ot_ptr = overdue_tasks.head;
				    while(temp_ot_ptr){
				        n_failed_tasks++;
				        temp_ot_ptr = temp_ot_ptr->next_node;
				    }

				    n_completed_tasks = n_total_tasks-(n_failed_tasks+n_running_tasks);
				    printf("TASK GENERATOR: %d failed, %d completed, %d still running.\n\r", n_failed_tasks, n_completed_tasks, n_running_tasks);

					break;
				case '.':
					if(Close()) {
						printf("Master Task Closed\n");
					}
					break;
				}
			}
		} else {
			_task_block();
		}

#ifdef PEX_USE_RTOS
	}
#endif
}

/*
** ===================================================================
**     Callback    : idle_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void idle_task(os_task_param_t task_init_data)
{
	printf("Idle Task created\n");

#ifdef PEX_USE_RTOS
  while (1) {
#endif
       //Do nothing forever
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/* END os_tasks */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
 ** @}
 */
/*
 ** ###################################################################
 **
 **     This file was created by Processor Expert 10.5 [05.21]
 **     for the Freescale Kinetis series of microcontrollers.
 **
 ** ###################################################################
 */
