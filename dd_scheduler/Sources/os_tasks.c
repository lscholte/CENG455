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
#include "scheduler_access.h"
#include <timer.h>
#include <math.h>


#ifdef __cplusplus
extern "C" {
#endif 

_pool_id message_pool;
WRITE_PRIVILEGE writePrivilege;
READ_PRIVILEGE readPrivilege[MAX_TASKS_WITH_READ_PERM];

/*
 * Prints a backspace character to the terminal
 */
void printBackspaceToTerminal() {
	char sequence[3] = { '\b', ' ', '\b' };
	UART_DRV_SendDataBlocking(myUART_IDX, sequence, sizeof(sequence), 1000);
}

/*
 * Removes the last character from the handler output buffer
 */
void printBackspaceToBuffer(char buffer[]) {
	int lastCharPosition = strlen(buffer) - 1;
	buffer[lastCharPosition] = 0;
}

/*
 * Removes any space characters from the end of a line in the terminal.
 *
 * Returns the index of the first non-space character in the buffer
 */
int removeTrailingSpacesFromTerminal(char buffer[]) {
	int charPos = strlen(buffer) - 1;
	while(charPos >= 0 && buffer[charPos] == ' ') {
		printBackspaceToTerminal();
		charPos--;
	}
	return charPos;
}

/*
 * Removes any space characters from the end of the handler output buffer.
 */
void removeTrailingSpacesFromBuffer(char buffer[]) {
	int charPos = strlen(buffer) - 1;
	while(charPos >= 0 && buffer[charPos] == ' ') {
		printBackspaceToBuffer(buffer);
		charPos--;
	}
}

/*
 * Deletes the last word from the terminal
 */
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

/*
 * Deletes the last word from the handler output buffer
 */
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

/*
 * Deletes an entire line from the terminal
 */
void printDeleteLineToTerminal(char buffer[]) {
	int lastCharPosition = strlen(buffer) - 1;
	int i;
	for(i = lastCharPosition; i >= 0; --i) {
		printBackspaceToTerminal();
	}
}

/*
 * Empties the handler output buffer
 */
void printDeleteLineToBuffer(char buffer[]) {
	memset(buffer, 0, BUFFER_LENGTH);
}

/*
 * Moves to the next line of the terminal
 */
void printNewlineToTerminal() {
	char sequence[2] = { '\n', '\r' };
	UART_DRV_SendDataBlocking(myUART_IDX, sequence, sizeof(sequence), 1000);
}

/*
 * Prints the specified character to the terminal
 */
void printCharacterToTerminal(char c) {
	UART_DRV_SendDataBlocking(myUART_IDX, &c, sizeof(char), 1000);
}


/*
 * Adds the character to the end of the handler output buffer
 */
bool printCharacterToBuffer(char c, char buffer[]) {
	int newCharPosition = strlen(buffer);
	if(newCharPosition >= BUFFER_LENGTH) {
		printf("Maximum line length reached. Cannot print character %c\n", c);
		return false;
	}
	buffer[newCharPosition] = c;
	return true;
}

/*
 * Check if the character c is a special character or a printable character.
 * Updates the terminal and the handler output buffer according to the type of the character
 */
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

/*
 * Prints a line of text to the terminal. This is used when
 * a user task wants to print a line of text
 */
void handleString(char *string, char buffer[]) {
	printDeleteLineToTerminal(buffer);
	UART_DRV_SendDataBlocking(myUART_IDX, string, strlen(string), 1000);
	char sequence[2] = { '\n', '\r' };
	UART_DRV_SendDataBlocking(myUART_IDX, sequence, sizeof(sequence), 1000);
	UART_DRV_SendDataBlocking(myUART_IDX, buffer, BUFFER_LENGTH, 1000);
}

/*
 * Gets the current time since the RTOS started in milliseconds
 */
uint32_t getCurrentTime() {
	TIME_STRUCT time_struct;
	_time_get(&time_struct);
	return time_struct.MILLISECONDS + time_struct.SECONDS * 1000;
}

/*
 * Gets the current time since the Application started in milliseconds
 */
uint32_t getApplicationTime() {
	TIME_STRUCT time_struct;
	_time_get_elapsed(&time_struct);
	return time_struct.MILLISECONDS + time_struct.SECONDS * 1000;
}

/*
 * Deletes the task with the given task ID from the active task list
 */
void deleteFromActiveTasks(_task_id tid, ACTIVE_TASK_LIST *active_tasks) {
	//Destroys the task if it has not already been completed.
	//Does not call the task's exit handler
	_task_destroy(tid);

	//Find the task to remove from the active task list
	TASK_NODE *task_node_ptr = NULL;
	for(task_node_ptr = active_tasks->head;
			task_node_ptr != NULL;
			task_node_ptr = task_node_ptr->next_node) {
		if(task_node_ptr->tid == tid) {
			break;
		}
	}

	//Remove the task from the active task list
	remove(active_tasks, task_node_ptr);
	task_node_ptr = NULL;
}

/*
 * Schedules the task with the soonest deadline to run if the deleted task
 * was the one that was currently running. Otherwise this does nothing because
 * there is no need to schedule a new task
 */
void rescheduleTasks(_task_id deletedTid, ACTIVE_TASK_LIST *active_tasks, TASK_NODE **running_task_node_ptr) {

	//If the deleted task is the one currently running
	if((*running_task_node_ptr)->tid == deletedTid) {

		//We need to schedule a new task to run by finding
		//the task with the soonest deadline
		//from the active task list.
		TASK_NODE *soonest_task_node_ptr = NULL;
		TASK_NODE *task_node_ptr = NULL;
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

		//Set running_task_node_ptr to that new active task
		*running_task_node_ptr = soonest_task_node_ptr;
	}
}

bool sendTaskList(TASK_LIST *task_list_ptr, GENERIC_MESSAGE_PTR msg_ptr) {
	msg_ptr->HEADER.TARGET_QID = msg_ptr->HEADER.SOURCE_QID;
	msg_ptr->HEADER.SOURCE_QID =  _msgq_get_id(0, SCHEDULER_QUEUE);

	//TODO: We really should send a copy of the active task list
	msg_ptr->DATA_PTR = task_list_ptr;

	return _msgq_send(msg_ptr);
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

	_mqx_uint newPriority = PRIORITY_OSA_TO_RTOS(GENERATORTASK_TASK_PRIORITY + 1U);
	_task_set_priority(_task_get_id(), newPriority, &temp);

	//The list of tasks that have exceeded their deadlines.
	OVERDUE_TASK_LIST overdue_tasks;
	overdue_tasks.head = NULL;

	//Active tasks is a list of tasks that have not exceeded their deadlines.
	//The list is not sorted according to their deadlines
	ACTIVE_TASK_LIST active_tasks;
	active_tasks.head = NULL;

	//Keeps track of the item in the active task list
	//corresponding to the task that is currently running
	TASK_NODE *running_task_node_ptr = NULL;

	uint32_t idle_time = 0;

#ifdef PEX_USE_RTOS
	while (1) {
#endif

		TASK_NODE *soonest_task_node_ptr = NULL;
		TASK_NODE *task_node_ptr = NULL;

		//Find the task that has the nearest deadline
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
		if(soonest_task_node_ptr == NULL) {
			//There are no active tasks, so wait for a message indefinitely

			uint32_t initial_time = getCurrentTime();
			msg_ptr = _msgq_receive(scheduler_qid, 0);
			uint32_t final_time = getCurrentTime();
			idle_time += final_time - initial_time;
		}
		else if(currentTimeMillis < soonest_task_node_ptr->absolute_deadline) {
			//The current time has not yet passed the soonest task's deadline.
			//Wait for a message to be received or until the deadline occurs
			shortestRelativeDeadline = soonest_task_node_ptr->absolute_deadline - currentTimeMillis;
			msg_ptr = _msgq_receive(scheduler_qid, shortestRelativeDeadline);
		}


		//Recalculate the current time because some time may have passed
		//since it was last obtained
		currentTimeMillis = getCurrentTime();

		if(msg_ptr != NULL) {
			if(msg_ptr->TYPE == TASK_CREATION_MESSAGE_TYPE) {
				TASK_CREATION_DATA_PTR data_ptr = (TASK_CREATION_DATA_PTR) msg_ptr->DATA_PTR;
				TASK_CREATION_DATA data = *data_ptr;

				_task_id tid = data.TASK_ID;
				uint32_t relativeDeadline = data.DEADLINE;

				free(data_ptr);
				_msg_free(msg_ptr);

				uint32_t absoluteDeadline = currentTimeMillis + relativeDeadline;

				//Allocate and initialize a new task node
				TASK_NODE *task_node_ptr = (TASK_NODE *) malloc(sizeof(TASK_NODE));
				task_node_ptr->creation_time = currentTimeMillis;
				task_node_ptr->absolute_deadline = absoluteDeadline;
				task_node_ptr->next_node = active_tasks.head;
				task_node_ptr->previous_node = NULL;
				task_node_ptr->tid = tid;

				//Add the task node to the front of the active task list
				addToFront(&active_tasks, task_node_ptr);

				_mqx_uint temp;

				//If new task deadline is sooner than currently running task deadline
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
					//This task has a later deadline than the running task,
					//so give this task a lower priority
					_task_set_priority(tid, WAITING_TASK_PRIORITY, &temp);
				}

			}

			if(msg_ptr->TYPE == TASK_DELETION_MESSAGE_TYPE) {
				TASK_DELETION_DATA_PTR data_ptr = (TASK_DELETION_DATA_PTR) msg_ptr->DATA_PTR;
				TASK_DELETION_DATA data = *data_ptr;

				_task_id tid = data.TASK_ID;

				free(data_ptr);
				_msg_free(msg_ptr);

				deleteFromActiveTasks(tid, &active_tasks);
				rescheduleTasks(tid, &active_tasks, &running_task_node_ptr);
			}

			if(msg_ptr->TYPE == ACTIVE_TASK_REQUEST_MESSAGE_TYPE) {

				TASK_LIST *data_ptr = (TASK_LIST *) msg_ptr->DATA_PTR;

				copy(&active_tasks, data_ptr);

				if(!sendTaskList(data_ptr, msg_ptr)) {
					printf("\nCould not send active task response message\n");
					_task_block();
				}
			}

			if(msg_ptr->TYPE == OVERDUE_TASK_REQUEST_MESSAGE_TYPE) {

				TASK_LIST *data_ptr = (TASK_LIST *) msg_ptr->DATA_PTR;

				copy(&overdue_tasks, data_ptr);

				if(!sendTaskList(data_ptr, msg_ptr)) {
					printf("\nCould not send overdue task response message\n");
					_task_block();
				}
			}

			if(msg_ptr->TYPE == IDLE_TIME_REQUEST_MESSAGE_TYPE) {
				uint32_t *data_ptr = (uint32_t *) msg_ptr->DATA_PTR;
				*data_ptr = idle_time;

				msg_ptr->HEADER.TARGET_QID = msg_ptr->HEADER.SOURCE_QID;
				msg_ptr->HEADER.SOURCE_QID =  _msgq_get_id(0, SCHEDULER_QUEUE);

				bool result = _msgq_send(msg_ptr);
				if (result != TRUE) {
					printf("\nCould not send a message\n");
					_task_block();
				}

			}

		}
		else {

			//Find all tasks in the active task list that have gone past their deadline
			TASK_NODE *task_node_ptr = NULL;
			for(task_node_ptr = active_tasks.head;
					task_node_ptr != NULL;
					task_node_ptr = task_node_ptr->next_node) {
				if(task_node_ptr->absolute_deadline <= currentTimeMillis) {

					//Create copy of the active task node to be inserted in the overdue task list
					TASK_NODE *overdue_task_node_ptr = (TASK_NODE *) malloc(sizeof(TASK_NODE));
					overdue_task_node_ptr->creation_time = task_node_ptr->creation_time;
					overdue_task_node_ptr->absolute_deadline = task_node_ptr->absolute_deadline;
					overdue_task_node_ptr->next_node = task_node_ptr->next_node;
					overdue_task_node_ptr->previous_node = task_node_ptr->previous_node;
					overdue_task_node_ptr->tid = task_node_ptr->tid;

					//Task is overdue, so delete the task from the scheduler
					deleteFromActiveTasks(task_node_ptr->tid, &active_tasks);
					rescheduleTasks(task_node_ptr->tid, &active_tasks, &running_task_node_ptr);

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
//    dd_tcreate(SLAVETASK_TASK, 500, 1000);
//    _time_delay(1000);
//    dd_tcreate(SLAVETASK_TASK, 1500, 1000);
//    dd_tcreate(SLAVETASK_TASK, 3000, 1000);
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
//
//    return;



	printf("Generator Task Created!\n");

	_queue_id generator_task_qid = _msgq_open(MSGQ_FREE_QUEUE, 0);
	OpenR(generator_task_qid);
	OpenW();
	char line[BUFFER_LENGTH_WITH_NULL];

//	uint32_t temp;
//	_task_id idle_tid = _task_create(0, IDLETASK_TASK, (uint32_t)(NULL));
//	_task_set_priority(idle_tid, IDLE_TASK_PRIORITY, &temp);

#ifdef PEX_USE_RTOS
	while (1) {
#endif

		if(_getline(line)) {
			if(strlen(line) > 0 ) {
				switch(line[0]) {
					case '~': {
						//Syntax: ~deadline,runtime
						//or ~ for default deadline and runtime
						if(strlen(line) > 1) {
							int dest_size = strlen(&line[1]);
							char dest[dest_size];
							strcpy(dest, &line[1]);

							int numbers[] = {
									atoi(strtok(dest, " ,")),
									atoi(strtok(NULL, " ,"))
							};

							dd_tcreate(SLAVETASK_TASK, numbers[0], numbers[1]);
						}
						else {
							dd_tcreate(SLAVETASK_TASK, 3000, 1000);
						}
						break;
					}

					case '!': {
						//Syntax: !period,runtime
						//or ! for default period and runtime
						if(strlen(line) > 1) {
							int dest_size = strlen(&line[1]);
							char dest[dest_size];
							strcpy(dest, &line[1]);

							int numbers[] = {
									atoi(strtok(dest, " ,")),
									atoi(strtok(NULL, " ,"))
							};
							PERIODIC_TASK_PARAMETERS *parameters = (PERIODIC_TASK_PARAMETERS *) malloc(sizeof(PERIODIC_TASK_PARAMETERS));
							parameters->PERIOD = numbers[0];
							parameters->EXECUTION_TIME = numbers[1];
							_task_create(0, PERIODICGENERATORTASK_TASK, parameters);
						}
						else {
							PERIODIC_TASK_PARAMETERS *parameters = (PERIODIC_TASK_PARAMETERS *) malloc(sizeof(PERIODIC_TASK_PARAMETERS));
							parameters->PERIOD = 3000;
							parameters->EXECUTION_TIME = 1000;
							_task_create(0, PERIODICGENERATORTASK_TASK, parameters);
						}
						break;
					}

					case '*': {

						ACTIVE_TASK_LIST active_tasks;
						OVERDUE_TASK_LIST overdue_tasks;

						if(dd_return_overdue_list(&overdue_tasks) != MQX_OK) {
							printf("error: failed to obtain the overdue task list!\n\r");
							_task_block();
						}

						if(dd_return_active_list(&active_tasks) != MQX_OK) {
							printf("error: failed to obtain the active task list!\n\r");
							_task_block();
						}

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

						printf("TASK GENERATOR: %d failed, %d still running.\n\r", n_failed_tasks, n_running_tasks);

						destroyList(&active_tasks);
						destroyList(&overdue_tasks);

						break;
					}

					case '@': {
						uint32_t idle_time = dd_return_idle_time();
						uint32_t app_time = getApplicationTime();
						int percent_idle = round(((float) idle_time / app_time) * 100);

						printf("IDLE TIME: %u APP TIME: %u IDLE PERCENT: %d%%\n", idle_time, app_time, percent_idle);
						break;
					}

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
	TIME_STRUCT time_struct;

	uint32_t counter = 0;
#ifdef PEX_USE_RTOS
  while (1) {
#endif
	  //Do absolutely nothing
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/*
** ===================================================================
**     Callback    : periodic_generator_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void periodic_generator_task(os_task_param_t task_init_data)
{
	PERIODIC_TASK_PARAMETERS *parameters = (PERIODIC_TASK_PARAMETERS *) task_init_data;
	uint32_t execution_time = parameters->EXECUTION_TIME;
	uint32_t period = parameters->PERIOD;

	free(parameters);
	parameters = NULL;

  
#ifdef PEX_USE_RTOS
  while (1) {
#endif
    
	dd_tcreate(SLAVETASK_TASK, period, execution_time);
    OSA_TimeDelay(period);

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
