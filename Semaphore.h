//
//  Semaphore.h
//  
//
//  Created by Gayatri Singh on 9/26/14.
//
//

#ifndef _Semaphore_h
#define _Semaphore_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

// Declarations for wrapper functions...
int AllocateSharedMemory(int n);
void* MapSharedMemory(int id);
int CreateSemaphoreSet(int n, short* vals);
void DeleteSemaphoreSet(int id);
void LockSemaphore(int id, int i);
void UnlockSemaphore(int id, int i);

// The various semaphores used in the program.


#if !defined(__GNU_LIBRARY__) || defined(_SEM_SEMUN_UNDEFINED)
union semun
{
    int val;                // value for SETVAL
    struct semid_ds* buf;   // buffer for IPC_STAT, IPC_SET
    unsigned short*  array; // array for GETALL, SETALL
    struct seminfo*  __buf; // buffer for IPC_INFO
}; #endif

#endif
