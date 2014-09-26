//
//  client.h
//  
//
//  Created by Gayatri Singh on 9/26/14.
//
//

#ifndef _client_h
#define _client_h

typedef struct request{
    int request_id;
    int pid;
    int memory_segment_key;
    struct request * next;
} request_param;

void initializeRequestParam(int request_id,int pid);

int callServiceAsync(result_struct *, request_param *, requestHandle*);

int callServiceSync(result_struct *, request_param *);

void enqueue(request_param *);

int waitForResults(requestHandle());

int doSomething();

void useResultBytes();

#endif
