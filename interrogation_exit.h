#ifndef INTERROGATION_EXIT_H
#define INTERROGATION_EXIT_H

#include "processus.h"

int interrogation_point (int val);

int running_or_stopped (processus_table* proc_table);

int exit_jsh (int val, processus_table* proc_table);

//void exit_jsh (int val);

#endif