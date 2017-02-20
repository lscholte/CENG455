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

		default: //printable character (this probably is probably a bad assumption to make)
			if(printCharacterToBuffer(c, buffer)) {
				printCharacterToTerminal(c);
			}
			break;
	}
	_mutex_unlock(&readPrivilegeMutex);
}

void handleString(char *string, char buffer[]) {
	printDeleteLineToTerminal(buffer);
	UART_DRV_SendDataBlocking(myUART_IDX, string, sizeof(string), 1000);
	char sequence[2] = { '\n', '\r' };
	UART_DRV_SendDataBlocking(myUART_IDX, sequence, sizeof(sequence), 1000);
	UART_DRV_SendDataBlocking(myUART_IDX, buffer, BUFFER_LENGTH, 1000);
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
**     Callback    : user_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void user_task(os_task_param_t task_init_data)
{
	printf("User Task 1 Created!\n");

	OpenW();
	int i = 0;
  
#ifdef PEX_USE_RTOS
  while (1) {
#endif
	  if(i > 5) {
		  Close();
	  }
    
	_putline(_msgq_get_id(0, HANDLER_QUEUE), "test");

	++i;
    OSA_TimeDelay(2000);
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/*
** ===================================================================
**     Callback    : user_task2
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void user_task2(os_task_param_t task_init_data)
{
	printf("User Task2 Created!\n");

	_queue_id user_task_qid = _msgq_open(MSGQ_FREE_QUEUE, 0);
	OpenR(user_task_qid);

	char line[BUFFER_LENGTH_WITH_NULL];
  
	int i = 0;

#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */

	  if(i > 5) {
		  Close();
	  }
    
		if(_getline(line)) {
			printf("Task2 Line Received: %s\n", line);
		} else {
			_task_block();
		}
    
    ++i;
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/*
** ===================================================================
**     Callback    : user_task3
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void user_task3(os_task_param_t task_init_data)
{
	printf("User Task3 Created!\n");

	_queue_id user_task_qid = _msgq_open(MSGQ_FREE_QUEUE, 0);
	OpenR(user_task_qid);
	OpenW();

	char line[BUFFER_LENGTH_WITH_NULL];

#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
    
		if(_getline(line)) {
			printf("Task3 Line Received: %s\n", line);
		}
    
    
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
