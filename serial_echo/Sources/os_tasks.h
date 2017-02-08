/* ###################################################################
**     Filename    : os_tasks.h
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
** @file os_tasks.h
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup os_tasks_module os_tasks module documentation
**  @{
*/         

#ifndef __os_tasks_H
#define __os_tasks_H
/* MODULE os_tasks */

#include "fsl_device_registers.h"
#include "clockMan1.h"
#include "pin_init.h"
#include "osa1.h"
#include "mqx_ksdk.h"
#include "uart1.h"
#include "fsl_mpu1.h"
#include "fsl_hwtimer1.h"
#include "MainTask.h"
#include "SerialTask.h"
#include "myUART.h"
#include "UserTask.h"
#include "UserTask2.h"
#include <message.h>
#include "access_functions.h"

#ifdef __cplusplus
extern "C" {
#endif 

#define HANDLER_QUEUE 8
#define PUTLINE_QUEUE 9
#define GETLINE_QUEUE 10
#define NUM_CLIENTS 1
#define CLIENT_QUEUE_BASE 11

#define BUFFER_LENGTH 32

typedef struct write_privilege {
	uint32_t task_id; //if this has a value of 0, then no task has write privileges
	_queue_id qid; //this will be initially set by the handler task and then never modified again
} WRITE_PRIVILEGE, * WRITE_PRIVILEGE_PTR;

typedef struct read_privilege {
	uint32_t task_id;
	_queue_number stream_no;
} READ_PRIVILEGE, * READ_PRIVILEGE_PTR;

extern _pool_id message_pool;
extern WRITE_PRIVILEGE writePrivilege;
extern READ_PRIVILEGE readPrivilege;

void handleCharacter(unsigned char c, unsigned char *buffer);
void printCharacter(unsigned char c, unsigned char buffer[]);
void printBackspace(unsigned char buffer[]);
void printDeleteLine(unsigned char buffer[]);
void printDeleteWord(unsigned char buffer[]);

/*
** ===================================================================
**     Callback    : serial_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void serial_task(os_task_param_t task_init_data);


/*
** ===================================================================
**     Callback    : user_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void user_task(os_task_param_t task_init_data);

/*
** ===================================================================
**     Callback    : user_task2
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void user_task2(os_task_param_t task_init_data);

/* END os_tasks */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

#endif 
/* ifndef __os_tasks_H*/
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
