#ifndef PROCESSUS_H
#define PROCESSUS_H

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

processus_table* allocate_processus_table(size_t capacity);

void free_processus_table(processus_table* table);

int add_processus (processus* processus, processus_table* table );

int delete_processus (processus* processus, processus_table* table);

//fonction qui alloue processus** av taille + une qui realloc + free + ajoute processus 
//fonc retire processus table et decale les autres


#endif