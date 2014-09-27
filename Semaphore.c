//
//  Semaphore.c
//  
//
//  Created by Gayatri Singh on 9/26/14.
//
//

/**
 * Allocates a shared memory segment.
 *
 * @param  n  Size (in bytes) of chunk to allocate.
 * @return Id of shared memory chunk.
 */
int AllocateSharedMemory(int n)
{
    assert(n > 0); /* Idiot-proof the call. */
    return shmget(IPC_PRIVATE, n, IPC_CREAT | SHM_R | SHM_W);
}

/**
 * Allocates a shared memory segment and associates the key with it
 * different keys are used for different priority queues of the server
 * Current design makes following associations:
 * k=1234: low priority queue
 * k=3456: med priority queue
 * k=5678: high priority queue
 */
int Allocate_Priority_Queue_Shared_Memory(int n, int k)
{
    assert(n > 0); /*Idiot-proof the call.*/
    key_t key = k;
    return shmget(key,n,0666);
}

/**
 * Maps a shared memory segment onto our address space.
 *
 * @param  id  Shared memory block to map.
 * @return Address of mapped block.
 */

void* MapSharedMemory(int id)
{
    void* addr;
    assert(id != 0); /* Idiot-proof the call. */
    addr = shmat(id, NULL, 0);  /* Attach the segment...
                                 shmctl(id, IPC_RMID, NULL); /* ...and mark it destroyed. */
    return addr;
}

void SetAllSemaphores(int id, short* vals)
{
    union semun arg;
    assert(vals != NULL);
    arg.array = vals;
    semctl(id, 0, SETALL, arg);
}


/**
 * Creates a new semaphore set.
 *
 * @param  n     Number of semaphores in set.
 * @param  vals  Default values to start off with.
 * @return Id of semaphore set.
 */
int CreateSemaphoreSet(int n, short* vals)
{
    union semun arg;
    int id;
    assert(n > 0);        /* You need at least one! */
    assert(vals != NULL); /* And they need initialising! */
    id = semget(IPC_PRIVATE, n, SHM_R | SHM_W);
    arg.array = vals;
    semctl(id, 0, SETALL, arg);
    return id;
}
/**
 * Frees up the given semaphore set.
 *
 * @param  id  Id of the semaphore group.
 */
void DeleteSemaphoreSet(int id)
{
    if (semctl(id, 0, IPC_RMID, NULL) == -1)
    {
        perror("Error releasing semaphore!");
        exit(EXIT_FAILURE);
    }
}
/**
 * Locks a semaphore within a semaphore set.
 * BAD: I AM ASSUMING LOCK SEMAPHORE IS IN A FOR LOOP
 * @param  id  Semaphore set it belongs to.
 * @param  i   Actual semaphore to lock.
 *
 * @note If it’s already locked, you’re put to sleep.
 */
void LockSemaphore(int id, int i)
{
    struct sembuf sb;
    sb.sem_num = i;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;
    semop(id, &sb, 1);
}
/**
 * Unlocks a semaphore within a semaphore set.
 *
 * @param  id  Semaphore set it belongs to.
 * @param  i   Actual semaphore to unlock.
 */
void UnlockSemaphore(int id, int i)
{
    struct sembuf sb;
    sb.sem_num = i;
    sb.sem_op = 1;
    sb.sem_flg = SEM_UNDO;
    semop(id, &sb, 1);
}