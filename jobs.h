#ifndef JOBS_H
#define JOBS_H

#include "processus.h" //???

void printJobs(processus *);
void printTableOfJobs(processus_table *);
int jobs(bool argument,processus_table * processus_table);
void maj_main_print(processus_table* proc_table);

#endif
