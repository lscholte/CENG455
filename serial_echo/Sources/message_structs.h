/*
 * message_structs.h
 *
 *  Created on: Feb 7, 2017
 *      Author: lscholte
 */

#ifndef SOURCES_MESSAGE_STRUCTS_H_
#define SOURCES_MESSAGE_STRUCTS_H_

#define CHAR_MESSAGE_TYPE 0 //Data should be a pointer to an unsigned char
#define STRING_MESSAGE_TYPE 1 //Data should be a pointer to an unsigned char *
#define REQUEST_WRITE_MESSAGE_TYPE //Data should be a pointer to a _task_id
#define REQUEST_WRITE_RESPONSE_MESSAGE_TYPE //Data should be a pointer to a _queue_id

typedef struct message_body {
	int TYPE;
	void *DATA;
} MESSAGE_BODY, * MESSAGE_BODY_PTR;

typedef struct generic_message {
	MESSAGE_HEADER_STRUCT HEADER;
	MESSAGE_BODY_PTR BODY_PTR;
} GENERIC_MESSAGE, * GENERIC_MESSAGE_PTR;

#endif /* SOURCES_MESSAGE_STRUCTS_H_ */
