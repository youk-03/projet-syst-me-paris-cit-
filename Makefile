CC=gcc
CFLAGS=-Wall 
LIBS = -lreadline
DEPS=forkexec.h interrogation_exit.h mystring.h prompt.h pwd.h cd.h job.h redirect.h jobs_command.h kill.h 
EXEC=jsh

all : jsh

%.o: %.c $(DEPS)
	$(CC) -c $<

jsh : kill.o jobs_command.o redirect.o job.o forkexec.o interrogation_exit.o mystring.o prompt.o pwd.o cd.o main.o
	$(CC) -o $@ $^ $(LIBS) 

clean :
	rm -rf $(EXEC) *.o