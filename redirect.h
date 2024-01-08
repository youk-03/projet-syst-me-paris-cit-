#ifndef REDIRECT_H
#define REDIRECT_H

#include "job.h"

argument* redirect (argument* arg, int redirection[3]);

int return_redirect(char * string);

int exec_command (argument* arg, job_table* job_table, int last_return, int shell_pgid, int shell_fd, int redirection[3], job* job);

int mpipe (char* line, job_table* job_table, int last_return, int shell_pgid, int redirection[3], job* job);

int is_return_redirect (argument* arg);


#endif 