/*
 * message_structs.h
 *
 *  Created on: Feb 7, 2017
 *      Author: lscholte
 */

#ifndef SOURCES_MESSAGE_STRUCTS_H_
#define SOURCES_MESSAGE_STRUCTS_H_

#define CHAR_MESSAGE_TYPE 0
#define STRING_MESSAGE_TYPE 1

typedef struct message_body {
	int TYPE;
	void *DATA;
} MESSAGE_BODY, * MESSAGE_BODY_PTR;

typedef struct generic_message {
	MESSAGE_HEADER_STRUCT HEADER;
	MESSAGE_BODY BODY;
} GENERIC_MESSAGE, * GENERIC_MESSAGE_PTR;

#endif /* SOURCES_MESSAGE_STRUCTS_H_ */
