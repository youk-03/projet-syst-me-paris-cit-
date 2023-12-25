#ifndef JOBS_COMMAND_H
#define JOBS_COMMAND_H

#include "job.h" 
#include <stdbool.h>

void print_table_of_certain_jobs(job_table *, int);
void print_jobs(job *, int); // 1 or 2
void print_table_of_jobs(job_table *);
int jobs(bool, char *, job_table *);
void maj_main_print(job_table* job_table, bool stdout);

#endif
