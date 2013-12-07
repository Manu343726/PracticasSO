#include <semaphore.h>


struct mi_cond_var {
    sem_t sem;
    int count;
    pthread_mutex_t mutex;
};

typedef struct mi_cond_var mi_cond_var_t;


struct mi_cond_var *var_cond_new( );
void var_cond_free(struct  mi_cond_var *vc);
void var_cond_signal(struct  mi_cond_var *vc);
int var_cond_wait(struct  mi_cond_var *vc, pthread_mutex_t* l);

