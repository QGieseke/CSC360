#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include "pthread.h"

#include "semaphore.h"

#define NUM_ITERATIONS 1000
#define SMOKE_COUNT 10

#ifdef VERBOSE
#define VERBOSE_PRINT(S, ...) printf (S, ##__VA_ARGS__);
#else
#define VERBOSE_PRINT(S, ...) ;
#endif

struct Agent {
  pthread_mutex_t mutex;
  pthread_cond_t  MT;
  pthread_cond_t  MP;
  pthread_cond_t  PT;
  pthread_cond_t  smoke;
  pthread_sem_t  init;
};

struct Agent* createAgent() {
  struct Agent* agent = malloc (sizeof (struct Agent));
  agent->mutex   = PTHREAD_MUTEX_INITIALIZER;
  agent->MT   = PTHREAD_COND_INITIALIZER;
  agent->PT   = PTHREAD_COND_INITIALIZER;
  agent->MP   = PTHREAD_COND_INITIALIZER;
  agent->smoke   = PTHREAD_COND_INITIALIZER;
  agent->init    = sem_init(agent->init, NULL, 1);
  return agent;
}

/**
 * You might find these declarations helpful.
 *   Note that Resource enum had values 1, 2 and 4 so you can combine resources;
 *   e.g., having a MATCH and PAPER is the value MATCH | PAPER == 1 | 2 == 3
 */
enum Resource            {    MATCH = 1, PAPER = 2,   TOBACCO = 4};
char* resource_name [] = {"", "match",   "paper", "", "tobacco"};

int signal_count [5];  // # of times resource signalled
int smoke_count  [5];  // # of times smoker with resource smoked



void* SmokerM(void* av){
	VERBOSE_PRINT("M created\n");
	struct Agent* a = av;
	while(1){
		pthread_mutex_lock(a->mutex);
		VERBOSE_PRINT ("M\n");
		sem_post(&(a->init));
		pthread_cond_wait(NULL, a->PT);
		VERBOSE_PRINT ("M smoked\n");
		smoke_count[PAPER]++;
		smoke_count[TOBACCO]++;
		pthread_cond_signal(&(a->smoke));
		pthread_mutex_unlock(a->mutex);
	}
}

void* SmokerP(void* av){
	VERBOSE_PRINT("P created\n");
	struct Agent* a = av;
	while(1){
		pthread_mutex_lock(a->mutex);
		VERBOSE_PRINT ("P\n");
		sem_post(&(a->init));
		pthread_cond_wait(NULL, a->MT);
		VERBOSE_PRINT("P smoked\n");
		smoke_count[MATCH]++;
		smoke_count[TOBACCO]++;
		pthread_cond_signal(&(a->smoke));
		pthread_mutex_unlock(a->mutex);
	}
}

void* SmokerT(void* av){
	VERBOSE_PRINT("T created\n");
	struct Agent* a=av;
	while(1){
		//pthread_mutex_unlock(a->init);
		pthread_mutex_lock(a->mutex);
		VERBOSE_PRINT ("T\n");
		sem_post(&(a->init));
		pthread_cond_wait(NULL, a->MP);
		VERBOSE_PRINT("T smoked\n");
		smoke_count[MATCH]++;
		smoke_count[PAPER]++;
		pthread_cond_signal(&(a->smoke));
		pthread_mutex_unlock(a->mutex);
	}
}
/**
 * This is the agent procedure.  It is complete and you shouldn't change it in
 * any material way.  You can re-write it if you like, but be sure that all it does
 * is choose 2 random reasources, signal their condition variables, and then wait
 * wait for a smoker to smoke.
 */
void* agent (void* av) {
  struct Agent* a = av;
  
      VERBOSE_PRINT("Agent loop starting\n");
      //pthread_mutex_lock(a->mutex);
	sem_wait(&(a->init));
      pthread_cond_t M;
	pthread_create(&M, NULL, SmokerM, a);
	VERBOSE_PRINT("Waiting on M\n");
	sem_wait(&(a->init));
      pthread_cond_t P;
	pthread_create(&P, NULL, SmokerP,  a);
	VERBOSE_PRINT("Waiting on P\n");
	sem_wait(&(a->init));
      pthread_cond_t T;
	pthread_create(&T, NULL, SmokerT,a);
	VERBOSE_PRINT("Waiting on T\n");
	sem_wait(&(a->init));

      VERBOSE_PRINT("Threads created\n");
    for (int i = 0; i < NUM_ITERATIONS; i++) {
      int r = random() % 3;
//	sleep(1);
      //pthread_mutex_lock(&(a->init));
      pthread_mutex_lock (a->mutex);
      if (r==0) {
        VERBOSE_PRINT ("match Paper available\n");
        pthread_cond_signal(&(a->MP));
	signal_count[MATCH]++;
	signal_count[PAPER]++;
      }
      if (r==1) {
        VERBOSE_PRINT ("paper Tobacco available\n");
	pthread_cond_signal(&(a->PT));
	signal_count[PAPER]++;
	signal_count[TOBACCO]++;
      }
      if (r==2) {
        VERBOSE_PRINT ("tobacco Match available\n");
	pthread_cond_signal(&(a->MT));
	signal_count[MATCH]++;
	signal_count[TOBACCO]++;
      }
      VERBOSE_PRINT ("agent is waiting for smoker to smoke\n");
      pthread_cond_wait (NULL, a->smoke);
      VERBOSE_PRINT ("Smoke recieved, destroying\n");
      //pthread_destroy(M);
      //pthread_destroy(P);
      //pthread_destroy(T);
      //sleep(1);
	VERBOSE_PRINT("threads destroyed\n");

	sem_post(&(a->init));
	
      pthread_mutex_unlock (a->mutex);
      //pthread_mutex_unlock(a->init);
    }
  return NULL;
}

int main (int argc, char** argv) {
  pthread_init (7);
  struct Agent*  a = createAgent();
  // TODO
  //pthread_t A = pthread_create(agent, a);
  //pthread_t M = pthread_create(SmokerM, a);
  //pthread_t P = pthread_create(SmokerP, a);
  //pthread_t T = pthread_create(SmokerT, a);
  pthread_t A;
  pthread_create(&A, NULL, agent, a);
  pthread_join(A, NULL);
  //pthread_join(M, NULL);
  //pthread_join(P, NULL);
  //pthread_join(T, NULL);
  assert (signal_count [MATCH]   == smoke_count [MATCH]);
  assert (signal_count [PAPER]   == smoke_count [PAPER]);
  assert (signal_count [TOBACCO] == smoke_count [TOBACCO]);
  assert (smoke_count [MATCH] + smoke_count [PAPER] + smoke_count [TOBACCO] == NUM_ITERATIONS * 2 );
  printf ("Smoke counts: %d matches, %d paper, %d tobacco\n",
          smoke_count [MATCH], smoke_count [PAPER], smoke_count [TOBACCO]);
}
