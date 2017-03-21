/*
 * task_list.c
 *
 *  Created on: Mar 18, 2017
 *      Author: sat
 */

#include <stdlib.h>
#include <stdio.h>
#include "task_list.h"


void addToFront(TASK_LIST *task_list, TASK_NODE *node_ptr) {

	TASK_NODE *head = task_list->head;
	if(head == NULL) {
		node_ptr->previous_node = NULL;
		node_ptr->next_node = NULL;
		task_list->head = node_ptr;
		task_list->tail = node_ptr;
	}
	else {
		head->previous_node = node_ptr;
		node_ptr->next_node = head;
		node_ptr->previous_node = NULL;
		task_list->head = node_ptr;
	}
}

void addToBack(TASK_LIST *task_list, TASK_NODE *node_ptr) {

	TASK_NODE *tail = task_list->tail;
	if(tail == NULL) {
		node_ptr->previous_node = NULL;
		node_ptr->next_node = NULL;
		task_list->head = node_ptr;
		task_list->tail = node_ptr;
	}
	else {
		node_ptr->next_node = NULL;
		node_ptr->previous_node = tail;
		tail->next_node = node_ptr;
		task_list->tail = node_ptr;

	}
}

void remove(TASK_LIST *task_list, TASK_NODE *node_ptr) {
	//Now remove the task from the active task list.
	//If task pointer is null, then it has already
	//been removed from the active task list
	if(node_ptr != NULL) {
		TASK_NODE *prev_node = node_ptr->previous_node;
		TASK_NODE *next_node = node_ptr->next_node;

		//Case 1: Empty list
		if(task_list->head == NULL) {
			return;
		}

		//Case 2: 1 item in list
		else if(task_list->head == node_ptr && task_list->tail == node_ptr) {
			task_list->head = NULL;
			task_list->tail = NULL;
		}

		//Case 3: deleting head of list
		else if(task_list->head == node_ptr) {
			task_list->head = next_node;
			next_node->previous_node = NULL;
		}

		//Case 4: deleting tail of list
		else if(task_list->tail == node_ptr) {
			task_list->tail = prev_node;
			prev_node->next_node = NULL;
		}

		//Case 5: deleting some node in middle of list
		else {
			prev_node->next_node = next_node;
			next_node->previous_node = prev_node;
		}
//		else if(prev_node != NULL) {
//			prev_node->next_node = next_node;
//		}
//		else if(next_node != NULL) {
//			next_node->previous_node = prev_node;
//		}
//
//		else if(prev_node == NULL && next_node == NULL) {
//			task_list->head = NULL;
//			task_list->tail = NULL;
//		}
//		if(node_ptr == task_list->head) {
//			task_list->head = next_node;
//		}
//		if(node_ptr == task_list->tail) {
//			task_list->tail = prev_node;
//		}
		free(node_ptr);
	}
}

void copy(TASK_LIST *original_list, TASK_LIST *copied_list) {

	copied_list->head = NULL;
	copied_list->tail = NULL;

	TASK_NODE *task_node_ptr;
	for(task_node_ptr = original_list->head;
			task_node_ptr != NULL;
			task_node_ptr = task_node_ptr->next_node) {

		TASK_NODE *copied_node_ptr = (TASK_NODE *) malloc(sizeof(TASK_NODE));
		memcpy(copied_node_ptr, task_node_ptr, sizeof(TASK_NODE));

		addToBack(copied_list, copied_node_ptr);
	}
}

void destroyList(TASK_LIST *list) {
	TASK_NODE *current_node = list->head;
	while(current_node != NULL) {
		TASK_NODE *node_to_delete = current_node;
		current_node = current_node->next_node;
		free(node_to_delete);
	}
	list->head = NULL;
	list->tail = NULL;
}




