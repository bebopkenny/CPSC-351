#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "msg.h"    /* For the message struct */

/* The size of the shared memory segment */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void* sharedMemPtr;

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory 
 * @param msqid - the id of the allocated message queue
 */
void init(int& shmid, int& msqid, void*& sharedMemPtr)
{
	/* TODO: 
        1. Create a file called keyfile.txt containing string "Hello world" (you may do
 	    so manually or from the code).
	2. Use ftok("keyfile.txt", 'a') in order to generate the key.
	3. Use will use this key in the TODO's below. Use the same key for the queue
	   and the shared memory segment. This also serves to illustrate the difference
 	   between the key and the id used in message queues and shared memory. The key is
	   like the file name and the id is like the file object.  Every System V object 
	   on the system has a unique id, but different objects may have the same key.
	*/
	key_t key = ftok("keyfile.txt", 'a');
	if (key == -1) {
		perror("ftok");
		exit(1);
	}

	
	/* TODO: Get the id of the shared memory segment. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE */
	shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, 0666 | IPC_CREAT);
	if (shmid < 0) {
		perror("shmget");
		exit(-1);
	}

	/* TODO: Attach to the shared memory */
	sharedMemPtr = shmat(shmid, NULL, 0);
	if (sharedMemPtr == (void*) -1) {
		perror("shmat");
		exit(1);
	}

	/* TODO: Attach to the message queue */
	msqid = msgget(key, 0666 | IPC_CREAT);
	if (msqid < 0) {
		perror("msgget");
		exit(1);
	}
	/* Store the IDs and the pointer to the shared memory region in the corresponding function parameters */
	
}

/**
 * Performs the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */
void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	/* TODO: Detach from shared memory */
	if (shmdt(sharedMemPtr) == -1) {
		perror("shmdt");
	}
}

/**
 * The main send function
 * @param fileName - the name of the file
 * @return - the number of bytes sent
 */
unsigned long sendFile(const char* fileName)
{

	/* A buffer to store message we will send to the receiver. */
	message sndMsg; 
	
	/* A buffer to store message received from the receiver. */
	ackMessage rcvMsg;
		
	/* The number of bytes sent */
	unsigned long numBytesSent = 0;
	
	/* Open the file */
	FILE* fp =  fopen(fileName, "r");

	/* Was the file open? */
	if(!fp)
	{
		perror("fopen");
		exit(-1);
	}
	
	/* Read the whole file */
	while(!feof(fp))
	{
		/* Read at most SHARED_MEMORY_CHUNK_SIZE from the file and
 		 * store them in shared memory.  fread() will return how many bytes it has
		 * actually read. This is important; the last chunk read may be less than
		 * SHARED_MEMORY_CHUNK_SIZE.
 		 */
		 size_t bytesRead = fread(sharedMemPtr, 1, SHARED_MEMORY_CHUNK_SIZE, fp);

		 if (ferror(fp)) {
			 perror("fread");
			 fclose(fp);
			 exit(-1);
		 }
		
		/* TODO: count the number of bytes sent. */		
		sndMsg.mtype = SENDER_DATA_TYPE;
        sndMsg.size  = bytesRead;
		numBytesSent += bytesRead;
		/* TODO: Send a message to the receiver telling him that the data is ready
 		 * to be read (message of type SENDER_DATA_TYPE).
 		 */
		if (msgsnd(msqid, &sndMsg, sizeof(sndMsg) - sizeof(long), 0) == -1) {
			perror("msgsnd");
			fclose(fp);
			exit(-1);
		}

		
		/* TODO: Wait until the receiver sends us a message of type RECV_DONE_TYPE telling us 
 		 * that he finished saving a chunk of memory. 
 		 */
		if (msgrcv(msqid, &rcvMsg, sizeof(rcvMsg) - sizeof(long), RECV_DONE_TYPE, 0) == -1) {
		perror("msgrcv");
		fclose(fp);
		exit(-1);
		}

		// If we read fewer than SHARED_MEMORY_CHUNK_SIZE bytes, break out of the loop
		if (bytesRead < SHARED_MEMORY_CHUNK_SIZE) {
		break;
		}
	}

	

	/** TODO: once we are out of the above loop, we have finished sending the file.
 	  * Lets tell the receiver that we have nothing more to send. We will do this by
 	  * sending a message of type SENDER_DATA_TYPE with size field set to 0. 	
	  */
	 sndMsg.size = 0;
	 if (msgsnd(msqid, &sndMsg, sizeof(sndMsg) - sizeof(long), 0) == -1) {
	   perror("msgsnd final");
	 }
   
	 // Close file
	 fclose(fp);
   
	 return numBytesSent;
   }

/**
 * Used to send the name of the file to the receiver
 * @param fileName - the name of the file to send
 */
void sendFileName(const char* fileName)
{
	/* Get the length of the file name */
	int fileNameSize = strlen(fileName);

	/* TODO: Make sure the file name does not exceed 
	 * the maximum buffer size in the fileNameMsg
	 * struct. If exceeds, then terminate with an error.
	 */
	if (fileNameSize >= MAX_FILE_NAME_SIZE) {
        fprintf(stderr, "File name too long (max %d chars allowed)\n", MAX_FILE_NAME_SIZE - 1);
        exit(-1);
    }

	/* TODO: Create an instance of the struct representing the message
	 * containing the name of the file.
	 */
	fileNameMsg fMsg;

	/* TODO: Set the message type FILE_NAME_TRANSFER_TYPE */
	fMsg.mtype = FILE_NAME_TRANSFER_TYPE;

	/* TODO: Set the file name in the message */
	strncpy(fMsg.fileName, fileName, MAX_FILE_NAME_SIZE);
    fMsg.fileName[MAX_FILE_NAME_SIZE - 1] = '\0';

	/* TODO: Send the message using msgsnd */
	if (msgsnd(msqid, &fMsg, sizeof(fMsg) - sizeof(long), 0) == -1) {
        perror("msgsnd fileName");
        exit(-1);
    }
}


int main(int argc, char** argv)
{
	
	/* Check the command line arguments */
	if(argc < 2)
	{
		fprintf(stderr, "USAGE: %s <FILE NAME>\n", argv[0]);
		exit(-1);
	}
		
	/* Connect to shared memory and the message queue */
	init(shmid, msqid, sharedMemPtr);
	
	/* Send the name of the file */
        sendFileName(argv[1]);
		
	/* Send the file */
	fprintf(stderr, "The number of bytes sent is %lu\n", sendFile(argv[1]));
	
	/* Cleanup */
	cleanUp(shmid, msqid, sharedMemPtr);
		
	return 0;
}