#ifndef FORKEXEC_H
#define FORKEXEC_H

#include <stdbool.h>
#include "mystring.h"

int forkexec(char *, char **); 

int forkexecBackground( char *, char **);

bool is_background(argument* arg);

#endif