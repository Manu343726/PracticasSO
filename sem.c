
#include "sem.h"
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>

/*-----------------------------------------------------------------------------------*/
struct counting_sem * counting_sem_new(int count)
{
	struct counting_sem *sem;
	
	sem = (struct counting_sem *)malloc(sizeof(struct counting_sem));
	if (sem != NULL) {
		sem->count = count;
		pthread_cond_init(&(sem->cond), NULL);
		pthread_mutex_init(&(sem->mutex), NULL);
	}
	return sem;
}
/*-----------------------------------------------------------------------------------*/


void counting_sem_free(struct counting_sem *sem)
{
	pthread_cond_destroy(&(sem->cond));
	pthread_mutex_destroy(&(sem->mutex));
	free(sem);
}

/*-----------------------------------------------------------------------------------*/

static int
cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex, int timeout)
{
	int tdiff;
	unsigned long sec, usec;
	struct timeval rtime1, rtime2;
	struct timespec ts;
	int retval;
	
	if (timeout > 0) {
		/* Get a timestamp and add the timeout value. */
		gettimeofday(&rtime1, NULL);
		sec = rtime1.tv_sec;
		usec = rtime1.tv_usec;
		usec += timeout % 1000 * 1000;
		sec += (int)(timeout / 1000) + (int)(usec / 1000000);
		usec = usec % 1000000;
		ts.tv_nsec = usec * 1000;
		ts.tv_sec = sec;
		
		retval = pthread_cond_timedwait(cond, mutex, &ts);
		
		if (retval == ETIMEDOUT) {
			return  _TIMEOUT_;
		} else {
			/* Calculate for how long we waited for the cond. */
			gettimeofday(&rtime2, NULL);
			tdiff = (rtime2.tv_sec - rtime1.tv_sec) * 1000 +
			(rtime2.tv_usec - rtime1.tv_usec) / 1000;
			
			if (tdiff <= 0) {
				return 0;
			}
			
			return tdiff;
		}
	} else {
		pthread_cond_wait(cond, mutex);
		return  _TIMEOUT_;
	}
}
/*-----------------------------------------------------------------------------------*/
int
timedout_counting_sem_wait(struct counting_sem *sem, int timeout)
{
	int time_needed = 0;
	
	
	pthread_mutex_lock(&(sem->mutex));
	
	while (sem->count <= 0) {
		if (timeout > 0) {
			time_needed = cond_wait(&(sem->cond), &(sem->mutex), timeout);
			
			if (time_needed == _TIMEOUT_) {
				pthread_mutex_unlock(&(sem->mutex));
				return _TIMEOUT_;
			}
			
		} else {
			cond_wait(&(sem->cond), &(sem->mutex), 0);
		}
	}
	sem->count--;
	pthread_mutex_unlock(&(sem->mutex));
	return time_needed;
}

/*-----------------------------------------------------------------------------------*/
int
counting_sem_wait(struct counting_sem *sem)
{	
	pthread_mutex_lock(&(sem->mutex));
	
	while (sem->count <= 0) {
		pthread_cond_wait(&(sem->cond), &(sem->mutex));
		
	}
	sem->count--;
	pthread_mutex_unlock(&(sem->mutex));
	return 0;
}

/*-----------------------------------------------------------------------------------*/
void
counting_sem_signal(struct counting_sem *sem)
{
	pthread_mutex_lock(&(sem->mutex));
	sem->count++;
	
    if (sem->count > 1) {
		sem->count = 1;
	}
	
	pthread_cond_broadcast(&(sem->cond));
	pthread_mutex_unlock(&(sem->mutex));
}

/*-----------------------------------------------------------------------------------*/


