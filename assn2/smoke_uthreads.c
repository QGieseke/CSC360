#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include "uthread.h"
#include "uthread_mutex_cond.h"
#include "uthread_sem.h"

#define NUM_ITERATIONS 1000
#define SMOKE_COUNT 10

#ifdef VERBOSE
#define VERBOSE_PRINT(S, ...) printf (S, ##__VA_ARGS__);
#else
#define VERBOSE_PRINT(S, ...) ;
#endif

struct Agent {
  uthread_mutex_t mutex;
  uthread_cond_t  MT;
  uthread_cond_t  MP;
  uthread_cond_t  PT;
  uthread_cond_t  smoke;
  uthread_sem_t  init;
};

struct Agent* createAgent() {
  struct Agent* agent = malloc (sizeof (struct Agent));
  agent->mutex   = uthread_mutex_create();
  agent->MT   = uthread_cond_create (agent->mutex);
  agent->PT   = uthread_cond_create (agent->mutex);
  agent->MP   = uthread_cond_create (agent->mutex);
  agent->smoke   = uthread_cond_create (agent->mutex);
  agent->init    = uthread_sem_create(1);
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
		uthread_mutex_lock(a->mutex);
		VERBOSE_PRINT ("M\n");
		uthread_sem_signal(a->init);
		uthread_cond_wait(a->PT);
		VERBOSE_PRINT ("M smoked\n");
		smoke_count[PAPER]++;
		smoke_count[TOBACCO]++;
		uthread_cond_signal(a->smoke);
		uthread_mutex_unlock(a->mutex);
	}
}

void* SmokerP(void* av){
	VERBOSE_PRINT("P created\n");
	struct Agent* a = av;
	while(1){
		uthread_mutex_lock(a->mutex);
		VERBOSE_PRINT ("P\n");
		uthread_sem_signal(a->init);
		uthread_cond_wait(a->MT);
		VERBOSE_PRINT("P smoked\n");
		smoke_count[MATCH]++;
		smoke_count[TOBACCO]++;
		uthread_cond_signal(a->smoke);
		uthread_mutex_unlock(a->mutex);
	}
}

void* SmokerT(void* av){
	VERBOSE_PRINT("T created\n");
	struct Agent* a=av;
	while(1){
		//uthread_mutex_unlock(a->init);
		uthread_mutex_lock(a->mutex);
		VERBOSE_PRINT ("T\n");
		uthread_sem_signal(a->init);
		uthread_cond_wait(a->MP);
		VERBOSE_PRINT("T smoked\n");
		smoke_count[MATCH]++;
		smoke_count[PAPER]++;
		uthread_cond_signal(a->smoke);
		uthread_mutex_unlock(a->mutex);
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
      //uthread_mutex_lock(a->mutex);
	uthread_sem_wait(a->init);
      uthread_cond_t M = uthread_create(SmokerM, a);
	VERBOSE_PRINT("Waiting on M\n");
	uthread_sem_wait(a->init);
      uthread_cond_t P = uthread_create(SmokerP, a);
	VERBOSE_PRINT("Waiting on P\n");
	uthread_sem_wait(a->init);
      uthread_cond_t T = uthread_create(SmokerT, a);
	VERBOSE_PRINT("Waiting on T\n");
	uthread_sem_wait(a->init);

      VERBOSE_PRINT("Threads created\n");
    for (int i = 0; i < NUM_ITERATIONS; i++) {
      int r = random() % 3;
//	sleep(1);
      //uthread_mutex_lock(a->init);
      uthread_mutex_lock (a->mutex);
      if (r==0) {
        VERBOSE_PRINT ("match Paper available\n");
        uthread_cond_signal(a->MP);
	signal_count[MATCH]++;
	signal_count[PAPER]++;
      }
      if (r==1) {
        VERBOSE_PRINT ("paper Tobacco available\n");
	uthread_cond_signal(a->PT);
	signal_count[PAPER]++;
	signal_count[TOBACCO]++;
      }
      if (r==2) {
        VERBOSE_PRINT ("tobacco Match available\n");
	uthread_cond_signal(a->MT);
	signal_count[MATCH]++;
	signal_count[TOBACCO]++;
      }
      VERBOSE_PRINT ("agent is waiting for smoker to smoke\n");
      uthread_cond_wait (a->smoke);
      VERBOSE_PRINT ("Smoke recieved, destroying\n");
      //uthread_destroy(M);
      //uthread_destroy(P);
      //uthread_destroy(T);
      //sleep(1);
	VERBOSE_PRINT("threads destroyed\n");

	uthread_sem_signal(a->init);
	
      uthread_mutex_unlock (a->mutex);
      //uthread_mutex_unlock(a->init);
    }
  return NULL;
}

int main (int argc, char** argv) {
  uthread_init (7);
  struct Agent*  a = createAgent();
  // TODO
  //uthread_t A = uthread_create(agent, a);
  //uthread_t M = uthread_create(SmokerM, a);
  //uthread_t P = uthread_create(SmokerP, a);
  //uthread_t T = uthread_create(SmokerT, a);
  uthread_t A = uthread_create(agent, a);
  uthread_join(A, NULL);
  //uthread_join(M, NULL);
  //uthread_join(P, NULL);
  //uthread_join(T, NULL);
  assert (signal_count [MATCH]   == smoke_count [MATCH]);
  assert (signal_count [PAPER]   == smoke_count [PAPER]);
  assert (signal_count [TOBACCO] == smoke_count [TOBACCO]);
  assert (smoke_count [MATCH] + smoke_count [PAPER] + smoke_count [TOBACCO] == NUM_ITERATIONS * 2 );
  printf ("Smoke counts: %d matches, %d paper, %d tobacco\n",
          smoke_count [MATCH], smoke_count [PAPER], smoke_count [TOBACCO]);
}
