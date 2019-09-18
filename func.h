#ifndef _FUNC_H_
#define _FUNC_H_

#include <pthread.h>
#include <semaphore.h>

struct Cell { // Cell structure. Only holds a status in this case, but more properties can be assigned
	int status;
};

#define WORLD_SIZE 1000 // The size of the world (WORLD SIZE * WORLD SIZE)
#define WORLD_ITERS 500 // Amount of times to iterate over the world

#define SUS_PC 85 // Percentage of cells to spawn susceptible
#define ZOM_PC 2 // Percentage of cells to spawn as zombies

// Storing state names to their integer representations
#define EMP 0
#define SUS 1
#define ZOM 2
#define REM 3

// Storing the percentages for the chance of susceptible cells to turn into zombies for when they have 1, 2, ... 5, zombie neighbours
#define ZOM1 5
#define ZOM2 10
#define ZOM3 25
#define ZOM4 50
#define ZOM5 75

// Global variables which will be shared between the main program and all the threads
sem_t *thr_sem; // Array of thread semaphores (1 for each thread)
sem_t main_sem; // Main semaphore used in the main program

pthread_mutex_t main_mutex; // Mutex used for updating the counter for the number of threads who have completed their job for a specified job number
pthread_mutex_t counter_mutex; // Mutex used to update the statistics for an iteration of the world: number of susceptible cells, number of zombies, and number of dead cells

int thread_count; // Holds the number of threads to create in the main program
int close_flag; // Holds 0 or 1, used for the main program to communiate to all threads for them to close themselves and exit out of their loops. It is initialised to 0, and the main program sets it to 1 when it is done iterating through the world
int iter_count; // Used to keep track of the number of iterations of the world that have been completed (iterator/index in a for loop)
int thr_counter; // Each thread increments this variable by 1 when they have completed their assigned job. They then "fall asleep" and wait for their semaphore to be called to do another round of calculations
int thr_job_num; // Main program uses this global variable to communicate to the threads which job they should be doing. When the threads are woken up using their semaphores, they read this global variable and depending on its value go down a specified route inside a switch statement

int numOfSus; // Global variable which is used to calculate the amount of susceptible cells in that iteration of the world
int numOfZom; // Global variable which is used to calculate the amount of zombie cells in that iteration of the world
int numOfRem; // Global variable which is used to calculate the amount of removed/dead cells in that iteration of the world

int thr_qty; // Variable which holds the number of cells/blocks of the world each thread should process when they are woken up by the main program. Example: 1000x1000 world: 1,000,000 cells. 1,000,000 / 8 (number of threads) = 125,000 <- the amount of cells each thread will go through

struct Cell theWorld[WORLD_SIZE][WORLD_SIZE]; // Global world array which keeps the state of the world
struct Cell copyWorld[WORLD_SIZE][WORLD_SIZE]; // Global world used to hold the updated cell statuses. When ALL calculations are done, this 'copy world' is cloned to be 'the world' for the next iteration. This is done to allow all threads to perform their jobs on an unedited world, while also save all the workk that has been done by faster threads.

void *Thread(void*); // Declaration of the Thread function (used in Thread.c)

#endif