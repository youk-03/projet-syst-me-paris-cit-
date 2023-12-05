CC=gcc
CFLAGS=-Wall 
LIBS = -lreadline
DEPS=forkexec.h interrogation_exit.h mystring.h prompt.h pwd.h cd.h processus.h
EXEC=jsh

all : jsh

%.o: %.c $(DEPS)
	$(CC) -c $<

jsh : forkexec.o interrogation_exit.o mystring.o prompt.o pwd.o cd.o main.o
	$(CC) -o $@ $^ $(LIBS) 

clean :
	rm -rf $(EXEC) *.o