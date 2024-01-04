#ifndef REDIRECT_H
#define REDIRECT_H

#include "job.h"

argument* redirect (argument* arg);

int return_redirect(char * string);

int exec_command (argument* arg, job_table* job_table, int last_return);

int mypipe (const char* line, job_table* job_table, int last_return);


#endif 