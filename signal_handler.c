#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "clients.h"
#include "server.h"
#include "mailbox.h"



unsigned char _pause = 1; // Shared variable to pause/resume execution
/// Semaphores to pause the signal_handler thread when waiting for signals
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static sys_mbox_t* gbl_mbox=NULL;	//Initialized upon thread creation

// Flag to finish the "endless" signal waiting loop
unsigned char finish_thread = 0;

/** Funciones para parar/continuar este hilo (gestor de señales)
 **/
void my_pause() {
	_pause = 1;
	pthread_mutex_lock(&lock);
    while(_pause==1) { /* We're paused */
        pthread_cond_wait(&cond, &lock); /* Wait for play signal */
    }
    pthread_mutex_unlock(&lock);
	
}

void my_resume() {
	pthread_mutex_lock(&lock);
	_pause=0;
	pthread_cond_signal(&cond); // cond to reevaluate pause
	pthread_mutex_unlock(&lock);
}




void SIGINT_handler(int signo) {
    
	pthread_t id;
	
    id = pthread_self();
    fprintf(logfile,"thread %u: caught signal %d (SIGINT)\n", (unsigned int) id, signo);
}


void* signal_handler_thread(void * c) {
#ifndef SYNCH_SIGNAL	
	struct sigaction act;
	sigset_t set;
	
	// Be sure to UNBLOCK all
	sigemptyset(&set);
	pthread_sigmask(SIG_SETMASK,&set,NULL);
		
	// Setup handler for INT signal
	act.sa_handler = SIGINT_handler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGINT, &act, NULL) != 0) {
        printf("ERROR: can't install SIGINT signal handler");
		return NULL;
	}
	
	//SETUP OTHER SIGNALS.... USR1??	
#endif
	
	
	// Wait for ever
	while (!finish_thread) {
		
#ifdef SYNCH_SIGNAL
		int sig;
		//Wait for any signal
		sigfillset(&set);
		sigwait(&set, &sig);
		 
		switch (sig) {
			case SIGINT:
				printf("SIGUSR2 received....\n"); SIGINT_handler(2);
				break;
			default:
				printf("UNEXPECTED signal (%d) received. \n",sig);
				break;
				
		}
#else		
		my_pause();

#endif
	}

	printf("Exiting signal_handler thread\n");
	return NULL;
	
}


int create_signal_handler(sys_mbox_t* mbox) {

	pthread_t tmp;
	int r;	

	//Initialize global reference to mailbox
	gbl_mbox=mbox;

	if ( ( r= pthread_create(&tmp,NULL, signal_handler_thread,NULL ) ) == 0)
		printf("Signal handler thread id: %u \n",(unsigned int)tmp);
	return r;
}
