CC = gcc
CFLAGS = -Wall
SRC = echo_client.o echo_server.o echo_server_sig.o echo_io.o
OBJ = $(SRC:.c=.o)

all: $(OBJ) 
	${CC} echo_client.o echo_io.o -o echo_client
	${CC} echo_server.o echo_io.o -o echo_server
	${CC} echo_server_sig.o echo_io.o -o echo_server_sig

echo_io.o: echo_io.h

echo_client.o: echo_io.h 

echo_server.o: echo_io.h

echo_server_sig.o: echo_io.h

clean:
	rm -f *.o core 

cleanall:
	rm -f *.o core echo_client echo_server echo_server_sig
