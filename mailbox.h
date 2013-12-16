
#ifndef _MI_MBOX_
#define _MI_MBOX_



// IMPLEMENTATION of a mailbox mechanism with our own counting sem. and locks.

#include "message.h"

#define SYS_MBOX_SIZE 32

//Global LOGFILE
extern FILE* logfile;


struct sys_mbox;
typedef struct sys_mbox sys_mbox_t;



/// Exportable functions
struct sys_mbox *mbox_new(unsigned int max_size);


void mbox_free(struct sys_mbox *mbox);
void  mbox_post( struct sys_mbox *mbox, void *msg);
void* mbox_fetch(struct sys_mbox *mbox);

void mbox_Multipost ( struct sys_mbox *mbox, void* msg_q[], int nmsg);
void mbox_post ( struct sys_mbox *mbox, void* msg );

#endif /* _MI_MBOX_ */

