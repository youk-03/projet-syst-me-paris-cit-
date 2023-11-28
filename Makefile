CC=gcc
CFLAGS=-Wall 
LIBS = -lreadline
DEPS=forkexec.h interrogation_exit.h mystring.h prompt.h pwd.h cd.h
EXEC=jsh

all : run

%.o: %.c $(DEPS)
	$(CC) -c $<

jsh : forkexec.o interrogation_exit.o mystring.o prompt.o pwd.o cd.o main.o
	$(CC) -o $@ $^ $(LIBS) 

run: $(EXEC)
	./$(EXEC) 

clean :
	rm -rf $(EXEC) *.o