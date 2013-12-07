#ifndef _MY_SEM
#define _MY_SEM

#include <pthread.h>
#include <errno.h>

#define _TIMEOUT_ 1000



// IMPLEMENTATION of counting semaphores with lock+var. cond

struct counting_sem {
	 int count;
	pthread_cond_t cond;
	pthread_mutex_t mutex;
};



typedef struct  counting_sem counting_sem_t;
 

 struct counting_sem *counting_sem_new(int count);
 void counting_sem_free(struct counting_sem *sem);   
void counting_sem_signal(struct counting_sem *sem);
int counting_sem_wait(struct counting_sem *sem);
int timedout_counting_sem_wait(struct counting_sem *sem, int timeout);
#endif //_MY_SEM
