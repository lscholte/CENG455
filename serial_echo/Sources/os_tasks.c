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
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif 

_pool_id message_pool;

void printBackspace(unsigned char buffer[]) {
	int lastCharPosition = strlen(buffer) - 1;
	buffer[lastCharPosition] = 0;
	char sequence[3] = { '\b', ' ', '\b' };
	UART_DRV_SendDataBlocking(myUART_IDX, sequence, sizeof(sequence), 1000);
}

//TODO: If there are multiple spaces in a row, this will not work properly
void printDeleteWord(unsigned char buffer[]) {
	int lastCharPosition = strlen(buffer) - 1;
	int i;
	for(i = lastCharPosition; i >= 0; --i) {
		if(buffer[i] == ' ') {
			printBackspace(buffer);
			break;
		}
		printBackspace(buffer);
	}
}

void printDeleteLine(unsigned char buffer[]) {
	int lastCharPosition = strlen(buffer) - 1;
	int i;
	for(i = lastCharPosition; i >= 0; --i) {
		printBackspace(buffer);
	}
}

void printCharacter(unsigned char c, unsigned char buffer[]) {
	int newCharPosition = strlen(buffer);
	if(newCharPosition >= BUFFER_LENGTH) {
		printf("Maximum line length reached. Cannot print character %c", c);
		return;
	}
	buffer[newCharPosition] = c;
	UART_DRV_SendDataBlocking(myUART_IDX, &c, sizeof(char), 1000);
}

void handleCharacter(unsigned char c, unsigned char *buffer) {
	switch(c) {
		case 0x08: //backspace
			printBackspace(buffer);
			break;
		case 0x17: //delete word
			printDeleteWord(buffer);
			break;
		case 0x15: //delete line
			printDeleteLine(buffer);
			break;
		default: //printable character (this probably is probably a bad assumption to make)
			if(isprint(c)) {
				printCharacter(c, buffer);
			}
			break;
	}
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

	CHAR_MESSAGE_PTR msg_ptr;


	_queue_id handler_qid = _msgq_open(HANDLER_QUEUE, 0);

	if (handler_qid == 0) {
		printf("\nCould not open the server message queue\n");
		_task_block();
	}

	message_pool = _msgpool_create(sizeof(CHAR_MESSAGE), NUM_CLIENTS, 0, 0);

	if (message_pool == MSGPOOL_NULL_POOL_ID) {
		printf("\nCount not create a message pool\n");
		_task_block();
	}

	//The buffer holding the line of text that is displayed
	//TODO: We should gracefully handle the situation where we
	//type a line of characters into the terminal that is longer
	//than BUFFER_LENGTH.
	unsigned char buffer[BUFFER_LENGTH] = { 0 };
    sprintf(buffer, "\n\rType here: ");

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
		/* Write your code here ... */

		//Wait for message from ISR
		msg_ptr = _msgq_receive(handler_qid, 0);
		if (msg_ptr == NULL) {
			printf("\nCould not receive a message\n");
			_task_block();
		}


        //TODO: Wrap following line inside an if statement that checks if there
        //are any User tasks that have read privileges. If no such tasks exist,
        //then we don't care about handling the received character, so we will discard it
		handleCharacter(msg_ptr->DATA, buffer);

		/* free the message */
		_msg_free(msg_ptr);

		OSA_TimeDelay(10);
		/* Example code (for task release) */




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
