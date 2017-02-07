/*
 * message_structs.h
 *
 *  Created on: Feb 7, 2017
 *      Author: lscholte
 */

#ifndef SOURCES_MESSAGE_STRUCTS_H_
#define SOURCES_MESSAGE_STRUCTS_H_

typedef struct char_message
{
   MESSAGE_HEADER_STRUCT   HEADER;
   unsigned char           DATA;
} CHAR_MESSAGE, * CHAR_MESSAGE_PTR;

typedef struct string_message
{
   MESSAGE_HEADER_STRUCT   HEADER;
   unsigned char           *DATA;
} STRING_MESSAGE, * STRING_MESSAGE_PTR;

//typedef struct openW_message
//{
//   MESSAGE_HEADER_STRUCT   HEADER;
//   _task_id           		DATA;
//} OPENW_MESSAGE, * OPENW_MESSAGE_PTR;

#endif /* SOURCES_MESSAGE_STRUCTS_H_ */
