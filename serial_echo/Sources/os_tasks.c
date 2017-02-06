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

void echoBackspace(unsigned char buffer[]) {
	int lastCharPosition = strlen(buffer) - 1;
	buffer[lastCharPosition] = 0;
	char sequence[3] = { '\b', ' ', '\b' };
	UART_DRV_SendDataBlocking(myUART_IDX, sequence, sizeof(sequence), 1000);
}

void echoDeleteWord(unsigned char buffer[]) {
	int lastCharPosition = strlen(buffer) - 1;
	int i;
	for(i = lastCharPosition; i >= 0; --i) {
		if(buffer[i] == ' ') {
			echoBackspace(buffer);
			break;
		}
		echoBackspace(buffer);
	}
}

void echoDeleteLine(unsigned char buffer[]) {
	int lastCharPosition = strlen(buffer) - 1;
	int i;
	for(i = lastCharPosition; i >= 0; --i) {
		echoBackspace(buffer);
	}
}

void echoCharacter(unsigned char c, unsigned char buffer[]) {
	int newCharPosition = strlen(buffer);
	buffer[newCharPosition] = c;
	UART_DRV_SendDataBlocking(myUART_IDX, &c, sizeof(char), 1000);
}

void printCharacter(unsigned char c, unsigned char *buffer) {
	switch(c) {
		case 0x08: //backspace
			echoBackspace(buffer);
			break;
		case 0x17: //delete word
			echoDeleteWord(buffer);
			break;
		case 0x15: //delete line
			echoDeleteLine(buffer);
			break;
		default: //printable character (this probably is probably a bad assumption to make)
			echoCharacter(c, buffer);
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
	bool            result;


	_queue_id server_qid = _msgq_open(HANDLER_QUEUE, 0);

	if (server_qid == 0) {
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

#ifdef PEX_USE_RTOS
	while (1) {
#endif
		/* Write your code here ... */

		//Wait for message from ISR
		msg_ptr = _msgq_receive(server_qid, 0);
		if (msg_ptr == NULL) {
			printf("\nCould not receive a message\n");
			_task_block();
		}


		printCharacter(msg_ptr->DATA, buffer);

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
