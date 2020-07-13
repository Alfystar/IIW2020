CC = gcc
CFLAGS = -Wall
SRC = client.o prefork_server_flock.o prefork_child_flock.o lock_fcntl.o prefork_server_nlock.o prefork_child_nlock.o prefork_server_pipe.o prefork_child_pipe.o web_child.o pr_cpu_time.o prefork_io.o 
OBJ = $(SRC:.c=.o)

all: $(OBJ) 
	${CC} client.o prefork_io.o -o client
	${CC} prefork_server_flock.o lock_fcntl.o prefork_child_flock.o web_child.o pr_cpu_time.o prefork_io.o -o prefork_flock
	${CC} prefork_server_nlock.o prefork_child_nlock.o web_child.o pr_cpu_time.o prefork_io.o -o prefork_nlock
	${CC} prefork_server_pipe.o prefork_child_pipe.o web_child.o pr_cpu_time.o prefork_io.o -o prefork_pipe

client.o: prefork_io.h

prefork_io.o: prefork_io.h

prefork_server_flock.o: prefork_flock.h

prefork_child_flock.o: prefork_flock.h

lock_fnctl.o: 

prefork_server_nlock.o: 

prefork_child_nlock.o: 

prefork_server_pipe.o: prefork_io.h

prefork_child_pipe.o: 

web_child.o: prefork_io.h

pr_cpu_time.o: 

clean:
	rm -f *.o core 

cleanall:
	rm -f *.o core client prefork_flock prefork_nlock prefork_pipe
