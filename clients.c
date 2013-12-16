#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "mailbox.h"
#include "message.h"
#include "clients.h"


static pthread_t clients[MAX_CLIENTS];
static int n_clients=-1;

int nr_secs=1;
int nr_msgs=3;
enum _message_types_ default_client_type=TIME;

struct client_arg {
	enum _message_types_ m_type;
	int n_messages;
    int messages_per_post;
	int period;
	sys_mbox_t* mbox;
};

int get_nclients() {
	return n_clients;
}

pthread_t* get_client_list() {
	return clients;
}

void clear_client_list() {
	n_clients=-1;
}

static void process_answer(message_t* msg) {

	switch (msg->tipo) {
		case RANDOM:
			if (msg->content) {
				int* tmp = (int*) msg->content;
				fprintf(logfile,"CLIENT: random %d\n",*tmp);
			}
			else
				fprintf(logfile,"ERROR: client found null content in RANDOM message\n");		
			break;
			
		case TIME:
			
			if (msg->content) {
				char* tmp = (char*) msg->content;
				fprintf(logfile,"CLIENT: time %s\n", tmp);
			}
			else
				fprintf(logfile,"ERROR: client found null content in TIME message\n");	
			break;
			
		case NONE:
			fprintf(logfile,"CLIENT. NONE messaged processed\n");
		default:
			break;
	}
	
}

// La función de entrada de un thread siempre debe tener este prototipo
// pero esto no impide recibir argumentos "complejos" y devolver cualquier tipo 
// de resultado
void* client_thread (void* ptr) {
	struct client_arg* arg;
	int i,j,k;
	message_t* msg;
	int mess_per_post=1;

	arg = (struct client_arg*) ptr;
	fprintf(logfile,"NEW client of type %d with period %d \n",arg->m_type, arg->period);
	
	// Asumo que el nuevo campo del tipo client_arg se llama mess_post
	mess_per_post = arg->mess_post;
	for (i=0;i<arg->n_messages; i+=mess_per_post) {
			// ARRAY DE MENSAJES
			void* msg_q[mess_per_post];
			for (j=0;j<mess_per_post; j++) {
				if (  ( msg=new_message(arg->m_type ) )  == NULL ) {
					fprintf(logfile,"Error when creating  message\n");
					pthread_exit(NULL);
				}
				msg_q[j]= (void*)msg;
			}

			// Llamada Multipost
			mbox_Multipost ( arg->mbox, msg_q, mess_per_post);

			// Para cada mensaje del array, esperar a que el servidor termine
			// y procesar el mensaje 				
			
			for( j = 0 ; j < mess_per_post ; ++j )
			{
				// Wait for server to finish the work
                counting_sem_wait(msg_q[j]->op_completed);
                
                
                // We can process and free the message
                process_answer(msg_q[j]);
                free_message(msg_q[j]);
			}


		// Sleep for period seconds before the next message
		sleep(arg->period);		
	}
	return NULL;
}





// Funcion que crea un hilo nuevo que ejecutara el codigo de un cliente
// El hilo comenzara su ejecucion en la funcion client_thread
// Es necesario pasar como argumento una variable de tipo struct client_arg
// inicializada a partir de los argumentos de la funcion
int create_client(sys_mbox_t* mbox, enum _message_types_ m_type, int total_messages , int messages_per_post , int period) {
	
	struct client_arg* argument;
	pthread_t tmp;
	
	if (n_clients >= MAX_CLIENTS -1) {
		fprintf(stderr,"ERROR: max number of clients already executed.\n");
		fprintf(stderr,"Please call wait_for_clients before starting a new one\n");
		return -1;
	}
	
	// Build the argument to the new thread	
	argument = (struct client_arg*) malloc(sizeof(struct client_arg));
    
    if( !argument )
        return EXIT_FAILURE;

	// INICIALIZA AQUI LOS CAMPOS DE argument
    argument->mbox              = mbox;
    argument->n_messages        = total_messages;
    argument->messages_per_post = messages_per_post;
    argument->m_type            = m_type;
    argument->period            = period;
        
	
	// CREACION DE HILO (CON COMPROBACION DE ERRORES)
	// EL IDENTIFICADOR DE HILO SE DEBE ALMACENAR EN LA VARIABLE tmp

    if( pthread_create( &tmp , NULL , client_thread , argument ) != 0 )
        return -1;
        
    
   	n_clients++;
	clients[n_clients] = tmp;
	
	return 0;	
}

void 
