/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : mqx_ksdk.c
**     Project     : serial_echo
**     Processor   : MK64FN1M0VLL12
**     Component   : MQX_KSDK
**     Version     : Component 1.2.0, Driver 01.00, CPU db: 3.00.000
**     Repository  : KSDK 1.3.0
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-02-27, 15:25, # CodeGen: 13
**
**     Copyright : 1997 - 2015 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file mqx_ksdk.c
** @version 01.00
*/         
/*!
**  @addtogroup mqx_ksdk_module mqx_ksdk module documentation
**  @{
*/         

/* MODULE mqx_ksdk. */

#include "mqx_ksdk.h"
#include "bsp.h"

/* Application specific header file */
#include "rtos_main_task.h"
#include "os_tasks.h"
#include "MainTask.h"
#include "Handler.h"
#include "UserTask.h"
#include "SlaveTask.h"
extern void * kernel_data_prv;
#if MQXCFG_PREALLOCATED_SYSTEM_STACKS
extern uint8_t mqx_interrupt_stack[];
#endif


#define TASK_TEMPLATE_LIST_END         {0, 0, 0, 0, 0, 0, 0}
  
  
/* MQX task template list */
const TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
  /* Task: MainTask */
  {
    /* Task number                    */  MAINTASK_TASK,
    /* Entry point                    */  (TASK_FPTR)main_task,
    /* Stack size                     */  MAINTASK_TASK_STACK_SIZE,
    /* Task priority                  */  (PRIORITY_OSA_TO_RTOS(MAINTASK_TASK_PRIORITY)),
    /* Task name                      */  MAINTASK_TASK_NAME,
    /* Task attributes                */  (MQX_AUTO_START_TASK),
    /* Task parameter                 */  (uint32_t)(NULL),
    /* Task time slice                */  (uint32_t)(0U)
  },       
  /* Task: Handler */
  {
    /* Task number                    */  HANDLER_TASK,
    /* Entry point                    */  (TASK_FPTR)handler_task,
    /* Stack size                     */  HANDLER_TASK_STACK_SIZE,
    /* Task priority                  */  (PRIORITY_OSA_TO_RTOS(HANDLER_TASK_PRIORITY)),
    /* Task name                      */  HANDLER_TASK_NAME,
    /* Task attributes                */  (0),
    /* Task parameter                 */  (uint32_t)(NULL),
    /* Task time slice                */  (uint32_t)(0U)
  },       
  /* Task: UserTask */
  {
    /* Task number                    */  USERTASK_TASK,
    /* Entry point                    */  (TASK_FPTR)user_task,
    /* Stack size                     */  USERTASK_TASK_STACK_SIZE,
    /* Task priority                  */  (PRIORITY_OSA_TO_RTOS(USERTASK_TASK_PRIORITY)),
    /* Task name                      */  USERTASK_TASK_NAME,
    /* Task attributes                */  (0),
    /* Task parameter                 */  (uint32_t)(NULL),
    /* Task time slice                */  (uint32_t)(0U)
  },       
  /* Task: SlaveTask */
  {
    /* Task number                    */  SLAVETASK_TASK,
    /* Entry point                    */  (TASK_FPTR)slave_task,
    /* Stack size                     */  SLAVETASK_TASK_STACK_SIZE,
    /* Task priority                  */  (PRIORITY_OSA_TO_RTOS(SLAVETASK_TASK_PRIORITY)),
    /* Task name                      */  SLAVETASK_TASK_NAME,
    /* Task attributes                */  (0),
    /* Task parameter                 */  (uint32_t)(NULL),
    /* Task time slice                */  (uint32_t)(0U)
  },       
  TASK_TEMPLATE_LIST_END
};

const _WEAK_SYMBOL(MQX_INITIALIZATION_STRUCT MQX_init_struct) =
{
    /* PROCESSOR_NUMBER                */  BSP_DEFAULT_PROCESSOR_NUMBER,
    /* [KPSDK-2559] Workaround to make TAD happy. With new version of TAD START_OF_KERNEL_MEMORY and END_OF_KERNEL_MEMORY can be removed */
    /* START_OF_KERNEL_MEMORY          */  &kernel_data_prv,
    /* END_OF_KERNEL_MEMORY            */  BSP_DEFAULT_END_OF_HEAP,
    /* INTERRUPT_STACK_SIZE            */  BSP_DEFAULT_INTERRUPT_STACK_SIZE,
#if MQX_CUSTOM_MAIN
    /* TASK_TEMPLATE_LIST              */  NULL,
#else
    /* TASK_TEMPLATE_LIST              */  (TASK_TEMPLATE_STRUCT_PTR)MQX_template_list,
#endif /* MQX_CUSTOM_MAIN */
    /* MQX_HARDWARE_INTERRUPT_LEVEL_MAX*/  BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX,
    /* MAX_MSGPOOLS                    */  BSP_DEFAULT_MAX_MSGPOOLS,
    /* MAX_MSGQS                       */  BSP_DEFAULT_MAX_MSGQS,
    /* IO_CHANNEL                      */  BSP_DEFAULT_IO_CHANNEL,
    /* IO_OPEN_MODE                    */  BSP_DEFAULT_IO_OPEN_MODE,
    /* START_OF_HEAP                   */  BSP_DEFAULT_START_OF_HEAP,
    /* END_OF_HEAP                     */  BSP_DEFAULT_END_OF_HEAP,
};

/* END mqx_ksdk. */

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
