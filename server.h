//
//  server.h
//  
//
//  Created by Gayatri Singh on 9/26/14.
//
//

#ifndef _server_h
#define _server_h

#define QUEUE_SIZE 50

typedef struct queue{
    request_param * head;
    request_param * end;
    int priority;
    int count;
} process_q;

typedef struct result{
    int result_id;
    int request_id;
    int pid;
    int * randomBytes;
} result_struct;

typedef struct serverProcess{
    int pid;
    sharedMem * low_queueRef;
    sharedMem * med_queueRef;
    sharedMem * high_queueRef;
    process_q * lowPriority;
    process_q * medPriority;
    process_q * highPriority;
} server;

extern server * randomBytesService;

extern typedef int (*requestHandle)();

void initializeRandomBytesService();

extern int returnBytes();

void initializeQueue();

request_param * dequeue();

void initializeResultParam();

#endif
