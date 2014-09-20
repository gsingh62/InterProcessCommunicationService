#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>

/*semaphore union*/
union semun{
	int val;
	struct semid_ds *buf;
	unsigned short int * array;
	struct seminfo *__buf;
};

/*Allocate a binary semaphore and obtain its id. If the semaphore already exists then the allocation is not required*/
int binary_semaphore_allocation(key_t key,int sem_flags){
	return semget(key,1,sem_flags);
}

/*Deallocate a binary semaphore. All users have finished their use. Return -1 on failure.*/
int binary_semaphore_deallocate(int semid){
	union semun ignored_argument;
	return semctl(semid, 1, IPC_RMID, ignored_argument);
}

/*Initialize a binary semaphore with a value of 1*/
int binary_semaphore_initialize(int semid){
	union semun argument;
	unsigned short values[1];
	values[0]=1;
	argument.array=values;
	return semctl(semid,0,SETALL, argument);
}

/*Wait on a binary semaphore. Block until the semaphore value is positive, then decrement it by 1.*/
int binary_semaphore_wait(int semid){
	struct sembuf operations[1];
	/*Use the first (and only) semaphore. */
	operations[0].sem_num=0;
	/*Decrement by 1*/
	operations[0].sem_op=-1;
	/*Permit undo'ing*/
	operations[0].sem_flg = SEM_UNDO;
	
	return semop(semid,operations,1);
}

/*Post to a binary semaphore: increment its value by 1. This returns immediately*/
int binary_semaphore_post(int semid){
	struct sembuf operations[1];
	/*Use the first (and only) semaphore. */
	operations[0].sem_num=0;
	/*Increment by 1*/
	operations[0].sem_op=1;
	/*Permit undo'ing*/
	operations[0].sem_flg = SEM_UNDO;
	
	return semop(semid,operations,1);
}

int main(void) {
    
    /* Our process ID and Session ID */
    pid_t pid, sid;
    
    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        printf("daemon did not create\n");
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then
     we can exit the parent process. */
    if (pid > 0) {
        printf("daemon was created\n");
        exit(EXIT_SUCCESS);
    }
    
    /* Change the file mode mask */
    umask(0);

    /* Open any logs here */
    openlog("daemonlog", LOG_PID|LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "A different kind of Hello world ... ");
    closelog();
    
    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        /* Log the failure */
        printf("the new child process did not get an SID\n");
        exit(EXIT_FAILURE);
    }
    
    
    
    /* Change the current working directory
    if ((chdir("/")) < 0) {
         Log the failure 
        printf("the new child process was not able to change the current working directory\n");
        exit(EXIT_FAILURE);
    } */
    
    /* Close out the standard file descriptors */
    //close(STDIN_FILENO);
    //close(STDOUT_FILENO);
    //close(STDERR_FILENO);
    
    /* Daemon-specific initialization goes here */
    
    /* The Big Loop */
    while (1) {
        /* Do some task here ... */
        int segment_id;
        char * shared_memory;
        struct shmid_ds shmbuffer;
        int segment_size;
        const int shared_segment_size = 0x6400;
        
        /*Allocate a shared memory segment*/
        segment_id = shmget(IPC_PRIVATE, shared_segment_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
        
        /*Attach the shared memory segment*/
        shared_memory = (char*) shmat(segment_id,0,0);
        printf("shared memory attached at address %p\n", shared_memory);
        
        /*Determine the segment's size*/
        shmctl(segment_id,IPC_STAT,&shmbuffer);
        segment_size = shmbuffer.shm_segsz;
        printf("segment size %d\n",segment_size);
        /*Write a string to the shared memory segment*/
        sprintf(shared_memory,"Hello World.");
        /*detach the shared memory segment*/
        shmdt(shared_memory);
        
        /*Reattach the shared memory segment at a different address*/
        shared_memory= (char*)shmat(segment_id, (void *) 0x5000000, 0);
        printf("shared memory reattached at %p\n",shared_memory);
        
        /*Print out the string from shared memory */
        printf("%s\n",shared_memory);
        
        /*Detach the shared memory segment*/
        shmdt(shared_memory);
        
        /*Deallocate the shared memory segment*/
        shmctl(segment_id,IPC_RMID,0);
        sleep(30); /* wait 30 seconds */
    }
    exit(EXIT_SUCCESS);
}