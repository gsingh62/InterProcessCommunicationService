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
    SEM_USER_1, // Indicates it’s the first person’s turn.
    SEM_USER_2  // Indicates it’s the second person’s turn.
};

int main(int argc, char* argv[])
{
    int idShMem;
    int idSem;
    char* buf;
    // Shared memory handle.
    // Semaphore set handle.
    // Shared memory buffer address.
    short vals[2]; // Values for initialising the semaphores.
    int   mySem;   // Semaphore indicating our user.
    int   yourSem; // Semaphore indicating the other user.
    puts("Welcome to CrapChat! Type ‘\\quit’ to exit.\n");
    // Get shared memory segment id off the command line.
    if (argc < 2)
    {
        // No segment id passed in, so we’ve got to create it.
        idShMem = AllocateSharedMemory(BUFSIZ);
        buf     = (char*) MapSharedMemory(idShMem);
        // We want each of the users to be blocked straight off
        // when they try to lock the shared memory area. When the
        // second user starts up, they’ll unlock the first so that
        // they can type. That’s what the zeros are for.
        vals[SEM_USER_1] = 0;
        vals[SEM_USER_2] = 0;
        idSem = CreateSemaphoreSet(2, vals);
        // Save the semaphore id in our shared memory so the other
        // user can get it.
        *((int*) buf) = idSem;
        // Record which semaphores we need to wait one and signal.
        mySem   = SEM_USER_1;
        yourSem = SEM_USER_2;
        
        // Write out the shared memory segment id so the other who
        // wants to chat with us can know.
        printf("You’re user one. Shared memory id is: %d\n",
               idShMem);
        puts("Waiting for user two...");
    } else {
        // We’ve a value! That means we’re the second user.
        idShMem = atoi(argv[1]);
        buf     = (char*) MapSharedMemory(isShMem);
        // Get the semaphore set id from shared memory.
        idSem = *((int*) buf);
        // Record which semaphores we need to wait one and signal.
        mySem   = SEM_USER_2;
        yourSem = SEM_USER_1;
        // Put an empty string in the shared memory.
        sprintf(buf, "");
        // Unlock the other user to signal they can talk.
        puts("You’re user two. Signalling to user one...");
        UnlockSemaphore(idSem, yourSem);
    }
    for (;;) {
        // Wait until it’s my turn to talk.
        LockSemaphore(idSem, semMe);
        // Did the other user exit?
        if (strcmp(buf, "\\quit\n") == 0)
            break;
        // Print the reply, if any.
        if (strlen(buf) > 0)
            printf("Reply: %s", buf);
        // Get your response.
        printf("> ");
        fgets(buf, BUFSIZ, stdin);
        // Hand over to the other user.
        UnlockSemaphore(idSem, semYou);
        // Do you want to exit?
        if (strcmp(buf, "\\quit\n") == 0)
            break;
    }
    // First user has to deallocate the semaphores.
    if (mySem == SEM_USER_1)
        DeleteSemaphoreSet(idSem);
}
