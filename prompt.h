#ifndef PROMPT_H
#define PROMPT_H

#include <stdbool.h>

char* create_prompt (char *path, int job_number);

int get_command(argument* arg);

bool is_redirect(argument* arg);

bool is_pipe(const char* line);

int digit_number (int number);

#endif