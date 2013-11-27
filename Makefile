CC = gcc

CFLAGS = -Wall -DEXERCISE_FINISHING 
##CFLAGS = -Wall -DEXERCISE_FINISHING  -DSYNCH_SIGNAL

LDFLAGS = -lpthread

TARGET = practica

OBJS = clients.o sem.o varcond.o cbuffer.o mailbox.o main.o message.o server.o signal_handler.o 

all: practica


practica: $(OBJS)
		$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET)  $(OBJS)

.c.o: 
	$(CC) $(CFLAGS) -c  $<

clean: 
		-rm -f *.o $(TARGET) 
