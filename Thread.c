#include <stdio.h>
#include <stdlib.h>

#include "func.h"

void *Thread(void *rank) {
	long my_rank = (long) rank; // Convert the threads rank into an actual long value
	int x,y,x2,y2,i,j; // Variables used when the thread is processing the world
	int start = my_rank * thr_qty; // Used to tell the thread where it's block of the world starts
	int end = start + thr_qty; // Used to tell the thread where it's block of the world ends
	int rnd, mySus, myZom, myRem; // Local (to each thread) counters used while going through its portion of the world
	int zomNeighbours, susNeighbours; // Keeps track of zombie and susceptible neighbour counts for each cell that the thread is looking at. This is used to alter the state of the current cell
	
	struct Cell c;
	
	while(close_flag != 1) { // Keep running, until the main program sets the close_flag to 1
		sem_wait(&thr_sem[my_rank]); // Wait for this threads semaphore to be called (by the main program)
		
		// Start with a clear counter for each iteration of the world
		mySus = 0;
		myZom = 0;
		myRem = 0;
		
		if(close_flag != 1) { // Double check that the main program does not want us to close. If not, continue to our instructed job
			switch(thr_job_num) { // Select and perform one job, specified by the main program via the global thr_job_num variable
				case 0: // Job Number 0: Initialise the world
					// Loop through this threads portion of the world (x AND y)
					for(y = start; y < end; y++) {
						for(x = 0; x < WORLD_SIZE; x++) {
							rnd = (rand() % 100) + 1; // Generate a random number for this cell (between 0 and 100)
							if(rnd <= SUS_PC) { // If the random number is less than or equal to (falling within the specified percentage range of 0-85% -> defined in the header file)
								// Set the current cell to be susceptible
								theWorld[x][y].status = SUS;
								copyWorld[x][y].status = SUS;
							}
							
							rnd = (rand() % 100) + 1; // Generate another random number (between 0 and 100) for the zombie chance
							if(rnd <= ZOM_PC) { // If the number falls within the specified zombie percentage chance, change this cell to a zombie
								theWorld[x][y].status = ZOM;
								copyWorld[x][y].status = ZOM;
							}
						} // for - x
					} // for - y
					break;
					
				case 1: // Job Number 1: Calculations - updates the value of the current cell based on the number of zombie and susceptible nodes around it (depending on the defined percentages in the header file)
					// Loop through this threads portion of the world (x AND y)
					for(y = start; y < end; y++) {
						for(x = 0; x < WORLD_SIZE; x++) {
							c = theWorld[x][y]; // Extract the current cell into a variable, as we access it multiple times
					
							if(c.status == SUS) { // If the current cell's status is Susceptible
								// Reset the neighbour counters to 0
								zomNeighbours = 0;
								susNeighbours = 0;
								
								// Go through the 8 cells around the current cells location (include wrapping to the other side of the world if it is a boundary cell), and update the counters
								if(theWorld[(x+1+WORLD_SIZE)%WORLD_SIZE][y].status == SUS) { susNeighbours++; } else if(theWorld[(x+1+WORLD_SIZE)%WORLD_SIZE][y].status == ZOM) { zomNeighbours++; }
								if(theWorld[(x-1+WORLD_SIZE)%WORLD_SIZE][y].status == SUS) { susNeighbours++; } else if(theWorld[(x-1+WORLD_SIZE)%WORLD_SIZE][y].status == ZOM) { zomNeighbours++; }
								if(theWorld[x][(y+1+WORLD_SIZE)%WORLD_SIZE].status == SUS) { susNeighbours++; } else if(theWorld[x][(y+1+WORLD_SIZE)%WORLD_SIZE].status == ZOM) { zomNeighbours++; }
								if(theWorld[x][(y-1+WORLD_SIZE)%WORLD_SIZE].status == SUS) { susNeighbours++; } else if(theWorld[x][(y-1+WORLD_SIZE)%WORLD_SIZE].status == ZOM) { zomNeighbours++; }
								
								if(theWorld[(x+1+WORLD_SIZE)%WORLD_SIZE][(y+1+WORLD_SIZE)%WORLD_SIZE].status == SUS) { susNeighbours++; } else if(theWorld[(x+1+WORLD_SIZE)%WORLD_SIZE][(y+1+WORLD_SIZE)%WORLD_SIZE].status == ZOM) { zomNeighbours++; }
								if(theWorld[(x-1+WORLD_SIZE)%WORLD_SIZE][(y-1+WORLD_SIZE)%WORLD_SIZE].status == SUS) { susNeighbours++; } else if(theWorld[(x-1+WORLD_SIZE)%WORLD_SIZE][(y-1+WORLD_SIZE)%WORLD_SIZE].status == ZOM) { zomNeighbours++; }
								if(theWorld[(x-1+WORLD_SIZE)%WORLD_SIZE][(y+1+WORLD_SIZE)%WORLD_SIZE].status == SUS) { susNeighbours++; } else if(theWorld[(x-1+WORLD_SIZE)%WORLD_SIZE][(y+1+WORLD_SIZE)%WORLD_SIZE].status == ZOM) { zomNeighbours++; }
								if(theWorld[(x+1+WORLD_SIZE)%WORLD_SIZE][(y-1+WORLD_SIZE)%WORLD_SIZE].status == SUS) { susNeighbours++; } else if(theWorld[(x+1+WORLD_SIZE)%WORLD_SIZE][(y-1+WORLD_SIZE)%WORLD_SIZE].status == ZOM) { zomNeighbours++; }
								
								rnd = (rand() % 100) + 1; // Generate another random number (between 0 - 100)
								if(rnd <= 1) { copyWorld[x][y].status = REM; } // 1% Chance of the cell dieing
								else { // Otherwise, updte the cells status depending on the amount of zombie neighbours that it has, and if the random number falls within the specified percentage
									if(zomNeighbours == 1 && rnd <= ZOM1) { copyWorld[x][y].status = ZOM; }
									else if(zomNeighbours == 2 && rnd <= ZOM2) { copyWorld[x][y].status = ZOM; }
									else if(zomNeighbours == 3 && rnd <= ZOM3) { copyWorld[x][y].status = ZOM; }
									else if(zomNeighbours == 4 && rnd <= ZOM4) { copyWorld[x][y].status = ZOM; }
									else if(zomNeighbours == 5 && rnd <= ZOM5) { copyWorld[x][y].status = ZOM; }
									else if(zomNeighbours > 5) { copyWorld[x][y].status = ZOM; }
								}
							}
						} // for - x
					} // for - y
					break;
					
				case 2: // Job Number 2: Save - saves the updates in the copy world into the actual world
					// Loop through this threads portion of the world (x AND y)
					for(y = start; y < end; y++) {
						for(x = 0; x < WORLD_SIZE; x++) {
							theWorld[x][y] = copyWorld[x][y]; // Copies the cell in the copy world at x,y location and saves it in the actual world at x,y
							
							// Also updates its local counter for this iteration of the number of susceptible, zombie, and removed cells
							if(copyWorld[x][y].status == SUS) {
								mySus++;
							} else if(copyWorld[x][y].status == ZOM) {
								myZom++;								
							} else if(copyWorld[x][y].status == REM) {
								myRem++;								
							}
						} // for - x
					} // for - y
					
					pthread_mutex_lock(&counter_mutex); // Lock the counter_mutex, then update the global counter for the number of susceptible, zombie, and removed cells
					numOfSus += mySus;
					numOfZom += myZom;
					numOfRem += myRem;
					pthread_mutex_unlock(&counter_mutex); // Unlock the mutex, so other threads can access it
					break;
			}
			
			pthread_mutex_lock(&main_mutex); // Lock the main_mutex, so the thread can increment the number of threads who have completed their assigned job
			thr_counter++;
			pthread_mutex_unlock(&main_mutex); // Unlock the mutex, so other threads can access it
			
			if(thr_counter == thread_count) sem_post(&main_sem); // If this is the last thread to complete the current job, wake up the main program using its semaphore
		}
	}
	
	return NULL;
}