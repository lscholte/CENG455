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
**         serial_task - void serial_task(os_task_param_t task_init_data);
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
READ_PRIVILEGE readPrivilege;

void printBackspaceToTerminal() {
	char sequence[3] = { '\b', ' ', '\b' };
	UART_DRV_SendDataBlocking(myUART_IDX, sequence, sizeof(sequence), 1000);
}

void printBackspaceToBuffer(unsigned char buffer[]) {
	int lastCharPosition = strlen(buffer) - 1;
	buffer[lastCharPosition] = 0;
}


//void printBackspace(unsigned char buffer[]) {
//	int lastCharPosition = strlen(buffer) - 1;
//	buffer[lastCharPosition] = 0;
//	char sequence[3] = { '\b', ' ', '\b' };
//	UART_DRV_SendDataBlocking(myUART_IDX, sequence, sizeof(sequence), 1000);
//}

void printDeleteWordToTerminal(unsigned char buffer[]) {
	int lastCharPosition = strlen(buffer) - 1;
	int i;
	for(i = lastCharPosition; i >= 0; --i) {
		if(buffer[i] == ' ') {
			printBackspaceToTerminal();
			break;
		}
		printBackspaceToTerminal();
	}
}

void printDeleteWordToBuffer(unsigned char buffer[]) {
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

////TODO: If there are multiple spaces in a row, this will not work properly
//void printDeleteWord(unsigned char buffer[]) {
//	int lastCharPosition = strlen(buffer) - 1;
//	int i;
//	for(i = lastCharPosition; i >= 0; --i) {
//		if(buffer[i] == ' ') {
//			printBackspace(buffer);
//			break;
//		}
//		printBackspace(buffer);
//	}
//}

void printDeleteLineToTerminal(unsigned char buffer[]) {
	int lastCharPosition = strlen(buffer) - 1;
	int i;
	for(i = lastCharPosition; i >= 0; --i) {
		printBackspaceToTerminal();
	}
}

void printDeleteLineToBuffer(unsigned char buffer[]) {
	memset(buffer, 0, BUFFER_LENGTH);
}

//void printDeleteLine(unsigned char buffer[]) {
//	int lastCharPosition = strlen(buffer) - 1;
//	int i;
//	for(i = lastCharPosition; i >= 0; --i) {
//		printBackspace(buffer);
//	}
//}

void printNewlineToTerminal() {
	char sequence[2] = { '\n', '\r' };
	UART_DRV_SendDataBlocking(myUART_IDX, sequence, sizeof(sequence), 1000);
}

//void printNewline(unsigned char buffer[]) {
//	memset(buffer, 0, BUFFER_LENGTH);
//	char sequence[2] = { '\n', '\r' };
//	UART_DRV_SendDataBlocking(myUART_IDX, sequence, sizeof(sequence), 1000);
//}

void printCharacterToTerminal(unsigned char c) {
	UART_DRV_SendDataBlocking(myUART_IDX, &c, sizeof(char), 1000);
}

bool printCharacterToBuffer(unsigned char c, unsigned char buffer[]) {
	int newCharPosition = strlen(buffer);
	if(newCharPosition >= BUFFER_LENGTH) {
		printf("Maximum line length reached. Cannot print character %c\n", c);
		return false;
	}
	buffer[newCharPosition] = c;
	return true;
}

//void printCharacter(unsigned char c, unsigned char buffer[]) {
//	int newCharPosition = strlen(buffer);
//	if(newCharPosition >= BUFFER_LENGTH) {
//		printf("Maximum line length reached. Cannot print character %c\n", c);
//		return;
//	}
//	buffer[newCharPosition] = c;
//	UART_DRV_SendDataBlocking(myUART_IDX, &c, sizeof(char), 1000);
//}

void handleCharacter(unsigned char c, unsigned char *buffer) {
	GENERIC_MESSAGE_PTR msg_ptr;
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
			//TODO: move to beginning of next line
			//TODO: send buffer to all user tasks that have read privileges

			//Allocate a string message
			msg_ptr = (GENERIC_MESSAGE_PTR) _msg_alloc(message_pool);
			if (msg_ptr == NULL){
				printf("\nCould not allocate a message\n");
				_task_block();
			}

			//Construct the message
			//NOTE: We must use malloc here because if we don't, then this memory
			//will be deallocated when this function terminates, and then when
			//the bufferCopy is received in _getline(), the pointer will no longer be valid
			char *bufferCopy = malloc(sizeof(char) * BUFFER_LENGTH);
			strcpy(bufferCopy, buffer);
			msg_ptr->BODY_PTR->TYPE = STRING_MESSAGE_TYPE;
			msg_ptr->BODY_PTR->DATA = &bufferCopy;
			msg_ptr->HEADER.TARGET_QID = readPrivilege.stream_no;
			msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(MESSAGE_BODY);

			//Send line to the handler
			bool result = _msgq_send(msg_ptr);
			if (result != TRUE) {
				printf("\nCould not send a message\n");
				_task_block();
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
}

void handleString(unsigned char *string) {
	//TODO: printDeleteLineToTerminal(buffer)
	//Send string to terminal + move to new line
	//send buffer string

	UART_DRV_SendDataBlocking(myUART_IDX, string, sizeof(string), 1000);
	char sequence[2] = { '\n', '\r' };
	UART_DRV_SendDataBlocking(myUART_IDX, sequence, sizeof(sequence), 1000);
}


/* User includes (#include below this line is not maintained by Processor Expert) */

/*
** ===================================================================
**     Callback    : serial_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void serial_task(os_task_param_t task_init_data)
{
	/* Write your local variable definition here */
	printf("serialTask Created!\n\r");

	_queue_id handler_qid = _msgq_open(HANDLER_QUEUE, 0);

	//TODO: Maybe need a mutex here?

	writePrivilege.qid = handler_qid;
	writePrivilege.task_id = MQX_NULL_TASK_ID;

	if (handler_qid == 0) {
		printf("\nCould not open the server message queue\n");
		_task_block();
	}

	message_pool = _msgpool_create(sizeof(GENERIC_MESSAGE), NUM_CLIENTS, 1, 0);

	if (message_pool == MSGPOOL_NULL_POOL_ID) {
		printf("\nCount not create a message pool\n");
		_task_block();
	}

	unsigned char buffer[BUFFER_LENGTH] = { 0 };

    //TODO: Create a data structure to store the user tasks that have read privileges.
    //The Handler task (ie this task) will maintain this structure. When a user task calls
    //OpenR, the Handler task will add the user task to the data structure. The structure
    //will contain the task ids and queue ids for the tasks that have read privileges.

	//OR

	//Make it a global data structure and just prevent concurrent accesses to it by using
	//mutexes

#ifdef PEX_USE_RTOS
	while (1) {
#endif
		//Check if there is a message from ISR (ie a keyboard character was pressed)
		GENERIC_MESSAGE_PTR msg_ptr = _msgq_receive(handler_qid, 0);
		if(msg_ptr != NULL) {
			MESSAGE_BODY_PTR msg_body_ptr = msg_ptr->BODY_PTR;
			//If this is a character message, then it is from ISR
			if(msg_body_ptr->TYPE == CHAR_MESSAGE_TYPE ) {
	//	        TODO: Wrap following line inside an if statement that checks if there
	//	        are any User tasks that have read privileges. If no such tasks exist,
	//	        then we don't care about handling the received character, so we will discard it
				unsigned char *ptr = (unsigned char *) msg_body_ptr->DATA;
				unsigned char c = *ptr;
				_msg_free(msg_ptr);
				handleCharacter(c, buffer);
			}

			//If this is a string message, it is from putline
			else if(msg_body_ptr->TYPE == STRING_MESSAGE_TYPE) {
				unsigned char **ptr = (unsigned char **) msg_body_ptr->DATA;
				unsigned char *line = *ptr;
				_msg_free(msg_ptr);
				handleString(line);
			}


		}

//	    OSA_TimeDelay(10);                 /* Example code (for task release) */



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
  /* Write your local variable definition here */
	printf("userTask Created!\n\r");

	//Creates a message queue with some available ID
	_queue_id user_task_qid = _msgq_open(MSGQ_FREE_QUEUE, 0);


	OpenR(user_task_qid);
	OpenW();

	unsigned char line[BUFFER_LENGTH];


  
#ifdef PEX_USE_RTOS
  while (1) {
#endif
    
	_putline(_msgq_get_id(0, HANDLER_QUEUE), "test");
	_getline(line);

	printf("Line Received: %s\n", line);
    
    OSA_TimeDelay(10);                 /* Example code (for task release) */
   
    
    
    
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
