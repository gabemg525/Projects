CC = gcc
CFLAGS = -Wall -g

TARGETS = mysh myserver
OBJS_SH = mysh.o myls.o mycat.o mywc.o server_command.o
OBJS_SERVER = myserver.o myls.o mycat.o mywc.o

all: $(TARGETS)

mysh: $(OBJS_SH)
	$(CC) $(CFLAGS) -o mysh $(OBJS_SH)

myserver: $(OBJS_SERVER)
	$(CC) $(CFLAGS) -o myserver $(OBJS_SERVER)

mysh.o: mysh.c myls.h mycat.h mywc.h server_command.h
	$(CC) $(CFLAGS) -c mysh.c

myserver.o: myserver.c myls.h mycat.h mywc.h
	$(CC) $(CFLAGS) -c myserver.c

myls.o: myls.c myls.h
	$(CC) $(CFLAGS) -c myls.c

mycat.o: mycat.c mycat.h
	$(CC) $(CFLAGS) -c mycat.c

mywc.o: mywc.c mywc.h
	$(CC) $(CFLAGS) -c mywc.c

server_command.o: server_command.c server_command.h
	$(CC) $(CFLAGS) -c server_command.c

clean:
	rm -f $(TARGETS) *.o
