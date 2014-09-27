//
//  client.c
//  
//
//  Created by Gayatri Singh on 9/26/14.
//
//

#include "client.h"

enum
{
    SEM_CLIENT, // Indicates it’s the client’s turn.
    SEM_SERVER  // Indicates it’s the server’s turn.
};


typedef struct request{
    int request_id;
    int async;
    pid_t pid;
    sharedMem * smref;
    struct request_param * next;
} request_param;

typedef struct sharedMemRef{
    int shared_Memory_key;
    int shared_Memory_semaphore_id;
}sharedMem;

typedef struct clientProcess{
    pid_t pid;
    request_param * requests;
} client;

void initializeRequestParam(int request_id,int pid){
}

client * initializeClient(client * cp){
    client * new = (client*)malloc(sizeof(client));
    new->pid = fork();
    return new;
}

/**
 * returns shared memory location
 */
int callServiceAsync(result_struct * rs, request_param * rp, int priority){
    short vals[2];
    int idShMem = AllocateSharedMemory(sizeof(result_struct));
    rs = (result_struct*) MapSharedMemory(idShMem);
    vals[SEM_CLIENT] = 0;
    vals[SEM_SERVER] = 1;
    int idSem = CreateSemaphoreSet(2,vals);
    rp->async = 0;
    (rp->smref)->shared_Memory_key = idShMem;
    (rp->sharedMem)->shared_Memory_semaphore_id = idSem;
    
    enqueue(rp,priority);
    
    return idShMem;
}

/**
 *returns actual bytes
 */
int callServiceSync(result_struct *, request_param *){
    short vals[2];
    int idShMem = AllocateSharedMemory(sizeof(result_struct));
    rs = (result_struct*) MapSharedMemory(idShMem);
    vals[SEM_CLIENT] = 0;
    vals[SEM_SERVER] = 1;
    int idSem = CreateSemaphoreSet(2,vals);
    rp->async = 1;
    (rp->smref)->shared_Memory_key = idShMem;
    (rp->sharedMem)->shared_Memory_semaphore_id = idSem;
    
    enqueue(rp,priority);
    
    return waitForResults(idSem,rs);
}

/**
 * priority must be either 1234, 4567 or 6789 corresponding to low, medium and high respectively.
 */
void enqueue(request_param * rp, int priority){
    int key = priority;
    int queue_key = Allocate_Priority_Queue_Shared_Memory(sizeof(process_q)*QUEUE_SIZE,key);
    process_q * priorityq = (process_q*)MapSharedMemory(queue_nkey);
    
    key = key+1
    int semaphore_key = Allocate_Priority_Queue_Shared_Memory(sizeof(sharedMem),key);
    sharedMem * queueRef = (sharedMem*)MapSharedMemory(semaphore_key);
    
    int queueSemaphore_Id = queueRef->shared_Memory_semaphore_id;
    
    LockSemaphore(queueSemaphore_Id,SEM_CLIENT);
    
    while(priorityq->count==50)
        sleep(10);
    
    if(priorityq->end==NULL){
        priorityq->head = rp;
        priorityq->end = rp;
    }
    
    else{
        request_param * end = priorityq->end;
        end->next = rp;
        priorityq->end = rp;
        rp->next=NULL;
    }
    priorityq->count++;
    
    UnlockSemaphore(queueSemaphore_Id,SEM_CLIENT);
}

int waitForResults(int idSem, result_struct * rs){
    LockSemaphore(idSem,SEM_CLIENT);
    int returnValue = *(rs->randomBytes + rs->request_id);
    UnlockSemaphore(idSem,SEM_CLIENT);
    return returnValue;
}

/**
 * This function deliberately loops through 10000 calls to rand() in order to give the illusion
 * of having work to do.
 */
int doSomething(){
    int randnum;
    
    for(i=0; i < 10000; i++)
        randnum = rand();
    
    return randnum;
}

/**
 * This function multiplies the bytes returned from doSomething() and server
 *
 */
void useResultBytes(int r1, int r2){
    if(r1==r2)
        return 1;
    else return 0;
}
