#ifndef _SERVER_
#define _SERVER_

#include "mailbox.h"


int create_server(sys_mbox_t* m);
//int wait_for_server_is_down(void);
int shutdown_server(void);
pthread_t getServerThId(void);
#endif