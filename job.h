#ifndef JOB_H
#define JOB_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>


struct job {
  pid_t job_pid;
  pid_t father_pid; //?? utile
  int status; // Running, Stopped, Detached, Killed ou Done (1,2,3,4,5)
  char* name; //(string ?)
  int id;
};
typedef struct job job;

struct job_table{

job** table;
size_t capacity;
int length;
};
typedef struct job_table job_table;

//job table

job_table* allocate_job_table(size_t capacity);

void free_job_table(job_table* table);

void add_job (job* job, job_table* table );

int delete_job (job* job, job_table* table);

int maj_job_table (job_table* job_table, bool stdout);

job* get_job (job_table* job_table, char* id);

//job

void free_job (job* job);

job* allocate_job (pid_t job_pid, pid_t father_pid, int status, char* name, bool keep_id);

//out

void increment_id();


#endif