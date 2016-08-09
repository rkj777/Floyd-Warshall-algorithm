/*
 * Eric Smith, Rajan Jassal
 * February 2016
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"
#include "Lab2IO.h"
#include <semaphore.h>

int thread_count_;
int **dp_;  //pointer to the distance array
int city_count_;

pthread_mutex_t** mutexes;
pthread_cond_t** condition_vars;

int** updated;

void* thread_subcal (void* rank);

int main (int argc, char* argv[])
{
   long thread_i;
   pthread_t* thread_handle_pt;
   double start, end;
   int i;
	int j;
    if (argc < 2) {
      printf("Please indicate the number of threads!\n");
      return 1;
   }
   thread_count_ = strtol(argv[1], NULL, 10);
    
   Lab2_loadinput(&dp_, &city_count_);

   thread_handle_pt=malloc(thread_count_*sizeof(pthread_t));
	
	//Allocating space for mutexes and condition variables 
	mutexes = malloc(city_count_ * sizeof(pthread_mutex_t*));
	condition_vars = malloc(city_count_ * sizeof(pthread_cond_t*));
	
	for(i = 0; i < city_count_ ; i++){
		mutexes[i] = malloc(city_count_ * sizeof(pthread_mutex_t));
		condition_vars[i] = malloc(city_count_ * sizeof(pthread_cond_t));
	}
	for(i = 0; i < city_count_ ; i++){
		for(j = 0; j < city_count_; j++){
			pthread_mutex_init(&mutexes[i][j],NULL);
			pthread_cond_init(&condition_vars[i][j], NULL);
		}
	}

	//Allocating and intializing the updated array
	updated = malloc(city_count_ * sizeof(int*));
	for(i = 0; i < city_count_ ; i++){
		updated[i] = (int*)calloc(city_count_ ,sizeof(int));
	}

   GET_TIME(start);

    //Create threads
   for (thread_i = 0; thread_i < thread_count_; ++thread_i)
      pthread_create(&thread_handle_pt[thread_i], NULL, thread_subcal, (void*)thread_i);
    
   //Join threads
   for (thread_i = 0; thread_i < thread_count_; ++thread_i)
      pthread_join(thread_handle_pt[thread_i], NULL);
    
   //Ending timestamp
   GET_TIME(end);
   printf("The elapsed time is %lfs.\n", end-start);
    
   //Destructors and memory releasing
	for(i = 0; i < city_count_ ; i++){
		for(j = 0; j < city_count_; j++){
			pthread_mutex_destroy(&mutexes[i][j]);
			pthread_cond_destroy(&condition_vars[i][j]);
		}
	}
	
	for(i = 0; i < city_count_ ; i++){
		free(mutexes[i]);
		free(condition_vars[i]);
		free(updated[i]);
	}
	
	free(mutexes);
	free(condition_vars);
	free(updated);
		
   Lab2_saveoutput(dp_, city_count_, end-start);
   DestroyMat(dp_, city_count_);

   return 0;
}

void* thread_subcal(void* rank){
    long myrank = (long)rank;
    int i, j, k, temp;
    
    for (k = 0; k < city_count_; ++k){
    	for (i = myrank * city_count_ / thread_count_; i < (myrank + 1) * city_count_ / thread_count_; ++i){
	  		for (j = 0; j < city_count_; ++j) {
	    		pthread_mutex_lock(&mutexes[k][j]);
				
            while(updated[k][j] < k-1){
					pthread_cond_wait(&condition_vars[k][j], &mutexes[k][j]);
				}
				
            if ((temp = dp_[i][k]+dp_[k][j]) < dp_[i][j]){
					dp_[i][j] = temp;
				}
				
				updated[i][j] = updated[i][j] + 1;
				pthread_cond_broadcast(&condition_vars[i][j]);
	 			pthread_mutex_unlock(&mutexes[k][j]);
	  		}
		}
    }
    return 0;
}
	

