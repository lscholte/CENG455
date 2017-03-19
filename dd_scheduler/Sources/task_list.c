/*
 * task_list.c
 *
 *  Created on: Mar 18, 2017
 *      Author: sat
 */

#include <stdlib.h>
#include "task_list.h"


void addToFront(TASK_LIST *task_list, TASK_NODE *node_ptr) {

	TASK_NODE *head = task_list->head;
	if(head == NULL) {
		node_ptr->previous_node = NULL;
		node_ptr->next_node = NULL;
		task_list->head = node_ptr;
	}
	else {
		head->previous_node = node_ptr;
		node_ptr->next_node = head;
		node_ptr->previous_node = NULL;
		task_list->head = node_ptr;
	}
}


void remove(TASK_LIST *task_list, TASK_NODE *node_ptr) {
	//Now remove the task from the active task list.
	//If task pointer is null, then it has already
	//been removed from the active task list
	if(node_ptr != NULL) {
		TASK_NODE *prev_node = node_ptr->previous_node;
		TASK_NODE *next_node = node_ptr->next_node;
		if(prev_node != NULL) {
			prev_node->next_node = next_node;
		}
		if(next_node != NULL) {
			next_node->previous_node = prev_node;
		}
		if(prev_node == NULL && next_node == NULL) {
			task_list->head = NULL;
		}
		free(node_ptr);
	}
}




