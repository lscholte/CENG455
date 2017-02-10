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

typedef struct string_message {
	MESSAGE_HEADER_STRUCT HEADER;
	int TYPE;
	char DATA[BUFFER_LENGTH_WITH_NULL];
} STRING_MESSAGE, * STRING_MESSAGE_PTR;

#endif /* SOURCES_MESSAGE_STRUCTS_H_ */
