#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>

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