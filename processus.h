#ifndef PROCESSUS_H
#define PROCESSUS_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

struct processus {
  pid_t process_pid;
  pid_t father_pid; //?? utile
  int status; // Running, Stopped, Detached, Killed ou Done (1,2,3,4,5)
  char* name; //(string ?)
  int id;
};
typedef struct processus processus;

struct processus_table{

processus** table;
size_t capacity;
int length;
};
typedef struct processus_table processus_table;

//processus table

processus_table* allocate_processus_table(size_t capacity);

void free_processus_table(processus_table* table);

int add_processus (processus* processus, processus_table* table );

int delete_processus (processus* processus, processus_table* table);

//processus

void free_processus (processus* proc);


#endif