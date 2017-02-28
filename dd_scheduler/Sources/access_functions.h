/*
 * access_functions.h
 *
 *  Created on: Feb 6, 2017
 *      Author: lscholte
 */

#ifndef SOURCES_ACCESS_FUNCTIONS_H_
#define SOURCES_ACCESS_FUNCTIONS_H_

#include <stdint.h>
#include <stdbool.h>
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"
#include <message.h>
#include "permission_structs.h"

//bool OpenR(uint16_t stream_no);
//bool _getline(char *line);
//_queue_id OpenW(void);
//bool _putline(_queue_id qid, char *line);
//bool Close(void);

_task_id dd_create(uint32_t template_index, uint32_t deadline);
uint32_t dd_delete(_task_id task_id); //returns status code
uint32_t dd_return_active_list(ACTIVE_TASK_LIST *list);
uint32_t dd_return_overdue_list(OVERDUE_TASK_LIST *list);

#endif /* SOURCES_ACCESS_FUNCTIONS_H_ */
