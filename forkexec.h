#ifndef FORKEXEC_H
#define FORKEXEC_H

#include <stdbool.h>
#include <unistd.h>
#include "mystring.h"
#include "job.h"

int forkexec(char *, char **, job_table*); 

int forkexecBackground( char *, char **);

bool is_background(argument* arg);

void reset_signal(int act);

void put_jsh_foreground (pid_t shell_pgid);

int put_foreground (job *job, pid_t pgid, job_table *job_table);

int put_background (job *j);

int wait_for_job (job *job, job_table *job_table, bool increment_id);

#endif