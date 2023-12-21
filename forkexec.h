#ifndef FORKEXEC_H
#define FORKEXEC_H

#include <stdbool.h>
#include "mystring.h"
#include "processus.h"

int forkexec(char *, char **, processus_table*); 

int forkexecBackground( char *, char **);

bool is_background(argument* arg);

#endif