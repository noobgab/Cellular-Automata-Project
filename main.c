/*
	Team Members:
	B00090592
	B00093117
	B00108141
*/

#include <stdio.h>
#include <stdlib.h>

#include "func.h"
#include "timer.h"

int main() {
	time_t t;
	srand((unsigned) time(&t));
	
	double start, finish, elapsed; // Variables which will hold time - used when calculating the amount of time the pgoram took to complete
	
	thread_count = 8; // Set the desired amount of threads to crete
	thr_counter = 0; // Initialise the thread counter global variable
	close_flag = 0; // Initialise the close flag variable to 0
	thr_qty = WORLD_SIZE / thread_count; // Calculate and store the amount of cells each thread is responsible for
	
	// Initialise the mutexes used in the program
	pthread_mutex_init(&main_mutex, NULL);
	pthread_mutex_init(&counter_mutex, NULL);
	
	sem_init(&main_sem, 0, 0); // Initialise the main semaphore
	thr_sem = malloc(thread_count * sizeof(sem_t)); // Allocataes memory for semaphores: 1 for each thread
	int si; // iterating variable for the for loop below
	for(si = 0; si < thread_count; si++) {
		sem_init(&thr_sem[si], 0, 0); // Initialise the semaphore for each thread
	} // 
	
	pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t)); // Allocate memory for the actual threads
	
	long rank; // Will be used in the for loop to allocate incremental rank numbers to each thread
	for(rank = 0; rank < thread_count; rank++) {
		pthread_create(&thread_handles[rank], NULL, Thread, (void *)rank); // Create threads with their unique rank number
	} // Create all the required threads
	
	GET_TIME(start); // Saves the current time in the start variable
	
	// Set the initial values for the required global variables
	numOfSus = 0;
	numOfZom = 0;
	numOfRem = 0;
	
	thr_job_num = 0; // Set the job number to 0, which will get the threads to initialise the world arrays
	thr_counter = 0; // Set the thread counter to 0, which is used to ensure that ALL the threads ahve completed this job
	for(si = 0; si < thread_count; si++) {
		sem_post(&thr_sem[si]); // Wake up all the threads using their individual semaphores
	}
	sem_wait(&main_sem); // Wait for the final thread to finish their job and wake up the main program
	printf("Sus,Zom,Rem\n"); // Print out the headings to the console
	
	thr_job_num = 2; // Set the job number to 2, which will get the threads to save the copy world into the main world
	thr_counter = 0; // Set the thread counter to 0
	for(si = 0; si < thread_count; si++) {
		sem_post(&thr_sem[si]); // Wake up all the threads using their individual semaphores
	}
	sem_wait(&main_sem); // Wait for the final thread to finish their job and wake up the main program
	printf("%d,%d,%d\n", numOfSus, numOfZom, numOfRem); // Print out the statistics of the initial state of the world
	
	for(iter_count = 0; iter_count < WORLD_ITERS; iter_count++) { // Begin iterating through the world for a specified amount of times
		// Reset the global statistic numbers
		numOfSus = 0;
		numOfZom = 0;
		numOfRem = 0;
		
		// Job Number 1 = calculate
		thr_job_num = 1; // Set the job number to 1
		thr_counter = 0; // Reset the thread counter
		for(si = 0; si < thread_count; si++) {
			sem_post(&thr_sem[si]); // Wake up all the threads using their individual semaphores
		}
		sem_wait(&main_sem); // Wait for all the threads to finish this job
		
		// Job number 2 = save
		thr_job_num = 2; // Set the job number to 2
		thr_counter = 0; // Reset the thread counter
		for(si = 0; si < thread_count; si++) {
			sem_post(&thr_sem[si]); // Wake up all the threads using their individual semaphores
		}
		sem_wait(&main_sem); // Wait for all the threads to finish this job
		
		if(iter_count % 50 == 0 || iter_count == WORLD_ITERS - 1) { // Every 50 iterations print out the statistics of the world - reduces the amount of information being printed to the screen
			printf("%d,%d,%d\n", numOfSus, numOfZom, numOfRem);
		}
		
	} // main program loop
	
	close_flag = 1; // Set the global close_flag to 1, which will instruct the threads to close
	for(si = 0; si < thread_count; si++) {
		sem_post(&thr_sem[si]); // Wake up all the threads using their individual semaphores
	}
	
	// Join threads - the program waits for each of the threads to finish and return
	for(rank = 0; rank < thread_count; rank++) {
		pthread_join(thread_handles[rank], NULL);
	} // pthread join
	
	GET_TIME(finish); // Grab the current time again
	
	// Clean up the memory by destroying mutexes and semaphores, and freeing up the memory that was allocated at the start of the program
	free(thread_handles);
	
	for(si = 0; si < thread_count; si++) {
		sem_destroy(&thr_sem[si]);
	}
	free(thr_sem);
	sem_destroy(&main_sem);
	
	pthread_mutex_destroy(&main_mutex);
	pthread_mutex_destroy(&counter_mutex);
	
	elapsed = finish - start; // Calculate the elapsed time
	printf("Elapsed time: %f seconds.\n", elapsed); // Print out the time it took to run this program
	
	return 0;
}