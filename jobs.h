#ifndef JOBS_H
#define JOBS_H

#include "processus.h" 
#include <stdbool.h>

void print_table_of_certain_jobs(processus_table *, int);
void print_jobs(processus *, int); // 1 or 2
void print_table_of_jobs(processus_table *);
int jobs(bool, char *, processus_table *);
void maj_main_print(processus_table* proc_table);

#endif
