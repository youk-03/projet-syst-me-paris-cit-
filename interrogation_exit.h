#ifndef INTERROGATION_EXIT_H
#define INTERROGATION_EXIT_H

#include "job.h"

int interrogation_point (int val);

int running_or_stopped (job_table* job_table);

int exit_jsh (int val, job_table* job_table);

#endif