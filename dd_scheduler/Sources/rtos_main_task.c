/* ###################################################################
**     Filename    : rtos_main_task.c
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
**         main_task - void main_task(os_task_param_t task_init_data);
**
** ###################################################################*/
/*!
** @file rtos_main_task.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup rtos_main_task_module rtos_main_task module documentation
**  @{
*/         
/* MODULE rtos_main_task */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"
#include "message_structs.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */

/* Initialization of Processor Expert components function prototype */
#ifdef MainTask_PEX_RTOS_COMPONENTS_INIT
extern void PEX_components_init(void);
#endif 

MUTEX_STRUCT readPrivilegeMutex;
MUTEX_STRUCT writePrivilegeMutex;

/*
** ===================================================================
**     Callback    : main_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void main_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
	printf("Main Task Created!\n");

	MUTEX_ATTR_STRUCT mutexAttribute;
	if(_mutatr_init(&mutexAttribute) != MQX_EOK) {
		printf("Failed to initialize mutex attribute\n");
		_task_block();
	}

	if(_mutex_init(&writePrivilegeMutex, &mutexAttribute) != MQX_EOK) {
		printf("Failed to initialize write privileges mutex\n");
		_task_block();
	}

	if(_mutex_init(&readPrivilegeMutex, &mutexAttribute) != MQX_EOK) {
		printf("Failed to initialize read privileges mutex\n");
		_task_block();
	}

//	_queue_id scheduler_qid = _msgq_open(SCHEDULER_QUEUE, 0);
//
//	message_pool = _msgpool_create(sizeof(GENERIC_MESSAGE), NUM_CLIENTS, 1, 0);
//
//	if (message_pool == MSGPOOL_NULL_POOL_ID) {
//		printf("Count not create a message pool\n");
//		_task_block();
//	}
  
  /* Initialization of Processor Expert components (when some RTOS is active). DON'T REMOVE THIS CODE!!! */
#ifdef MainTask_PEX_RTOS_COMPONENTS_INIT
  PEX_components_init(); 
#endif 
  /* End of Processor Expert components initialization.  */

#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
    
    
    OSA_TimeDelay(10);                 /* Example code (for task release) */
   
    
    
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/* END rtos_main_task */

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
