//
//  server.h
//  
//
//  Created by Gayatri Singh on 9/26/14.
//
//

#ifndef _server_h
#define _server_h

typedef struct queue{
    request_param * head;
    request_param * end;
    int priority;
} process_q;

typedef struct result{
    int result_id;
    int request_id;
    int pid;
    int randomBytes;
} result_struct;

typedef int (*requestHandle)();

int returnBytes();

void initializeQueue();

request_param * dequeue();

void initializeResultParam();

#endif
