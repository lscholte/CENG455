/*
 * scheduler_access.h
 *
 *  Created on: Mar 18, 2017
 *      Author: sat
 */

#ifndef SOURCES_SCHEDULER_ACCESS_H_
#define SOURCES_SCHEDULER_ACCESS_H_

_task_id dd_tcreate(uint32_t template_index, uint32_t deadline);
_task_id dd_tcreate_test(uint32_t template_index, uint32_t deadline, uint32_t runtime);
uint32_t dd_delete(_task_id tid); //returns status code
uint32_t dd_return_active_list(ACTIVE_TASK_LIST *list);
uint32_t dd_return_overdue_list(OVERDUE_TASK_LIST *list);
uint32_t dd_return_idle_time();

#endif /* SOURCES_SCHEDULER_ACCESS_H_ */
