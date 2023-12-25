#ifndef FORKEXEC_H
#define FORKEXEC_H

#include <stdbool.h>
#include "mystring.h"
#include "job.h"

int forkexec(char *, char **, job_table*); 

int forkexecBackground( char *, char **);

bool is_background(argument* arg);

#endif