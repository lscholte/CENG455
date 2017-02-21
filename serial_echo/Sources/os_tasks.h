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
**         handler_task - void handler_task(os_task_param_t task_init_data);
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
#include "Handler.h"
#include "myUART.h"
#include "UserTask.h"
#include "UserTask2.h"
#include "UserTask3.h"
#include "SlaveTask.h"
#include <message.h>
#include <mutex.h>
#include "access_functions.h"
#include "permission_structs.h"

#ifdef __cplusplus
extern "C" {
#endif 

#define HANDLER_QUEUE 8
#define PUTLINE_QUEUE 9
#define GETLINE_QUEUE 10
#define NUM_CLIENTS 1
#define CLIENT_QUEUE_BASE 11

#define BUFFER_LENGTH 32
#define BUFFER_LENGTH_WITH_NULL (BUFFER_LENGTH+1)

#define MAX_TASKS_WITH_READ_PERM 10

extern _pool_id message_pool;
extern WRITE_PRIVILEGE writePrivilege;
extern READ_PRIVILEGE readPrivilege[MAX_TASKS_WITH_READ_PERM];

/*
** ===================================================================
**     Callback    : handler_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void handler_task(os_task_param_t task_init_data);


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

/*
** ===================================================================
**     Callback    : user_task3
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void user_task3(os_task_param_t task_init_data);

/*
** ===================================================================
**     Callback    : slave_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void slave_task(os_task_param_t task_init_data);

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
