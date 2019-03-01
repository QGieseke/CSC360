#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "pthread.h"

#define MAX_ITEMS 10
const int NUM_ITERATIONS = 2000;
const int NUM_CONSUMERS  = 4;
const int NUM_PRODUCERS  = 4;

int producer_wait_count;     // # of times producer had to wait
int consumer_wait_count;     // # of times consumer had to wait
int histogram [MAX_ITEMS+1]; // histogram [i] == # of times list stored i items

int items = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


void* producer (void* v) {
  printf("producer started\n");
  for (int i=0; i<NUM_ITERATIONS; i++) {
    	// TODO
	pthread_mutex_lock(&lock);
	
	if(items < MAX_ITEMS){
		items++;
		printf("%d ", items);
		histogram[items]++;
	} else {
		i--;
		producer_wait_count++;
	}
	pthread_mutex_unlock(&lock);
	
  }
  return NULL;
}

void* consumer (void* v) {
  printf("consumer started\n");
  for (int i=0; i<NUM_ITERATIONS; i++) {
    	// TODO
	
	pthread_mutex_lock(&lock);
	if(items > 0) {
		items--;
		printf("%d ", items);
		histogram[items]++;
	} else {
		i--;	
		consumer_wait_count++;
	}
	pthread_mutex_unlock(&lock);
  }
  return NULL;
}

int main (int argc, char** argv) {
  //pthread_mutex_init(&lock, NULL);
  pthread_t t[NUM_PRODUCERS + NUM_CONSUMERS];

  //uthread_init (NUM_PRODUCERS+NUM_CONSUMERS);
  // TODO: Create Threads and Join
  
  for(int i = 0; i < NUM_CONSUMERS + NUM_PRODUCERS; i++){
	printf("creating %d\n", i);
	if(i < NUM_CONSUMERS){
		pthread_create(&t[i], NULL, consumer, NULL);
	} else {
		pthread_create(&t[i], NULL, producer, NULL);
	}
  }
  
  for(int i = 0; i < NUM_CONSUMERS+NUM_PRODUCERS; i++){
	pthread_join(t[i], NULL);
	printf("%d thread joined\n", i);
  }
  
  printf ("producer_wait_count=%d\nconsumer_wait_count=%d\n", producer_wait_count, consumer_wait_count);
  printf ("items value histogram:\n");
  int sum=0;
  for (int i = 0; i <= MAX_ITEMS; i++) {
    printf ("  items=%d, %d times\n", i, histogram [i]);
    sum += histogram [i];
  }
  assert (sum == sizeof (t) / sizeof (pthread_t) * NUM_ITERATIONS);
}
