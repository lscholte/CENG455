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

bool OpenR(uint16_t stream_no);
bool _getline(char * line);
_queue_id OpenW(void);
bool _putline(_queue_id qid, char * line);
bool Close(void);

#endif /* SOURCES_ACCESS_FUNCTIONS_H_ */
