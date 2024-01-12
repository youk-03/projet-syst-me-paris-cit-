#ifndef JOB_H
#define JOB_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#define MAX_PIPE 150


struct job {
  pid_t job_pid;
  pid_t father_pid; //?? utile
  int status; // Running, Stopped, Detached, Killed ou Done (1,2,3,4,5)
  char* name; //(string ?)
  int id;
 struct job** process_table;
  int process_number; //only for pipeline
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

void free_job (job *job);

job* allocate_job (pid_t job_pid, pid_t father_pid, int status, char* name, bool keep_id);

bool is_killed_or_done(job *job,int status); //regarde si ils sont tous killed ou done

bool is_stopped(job *job);//regarde si un est stoppé a appeler après is killed !

int delete_from_process_table(job* jobs, job* job_to_delete);

int maj_process_table(job* job);

int delete_killed_process(job* jobs);

int allocate_process_table(job* job);

//out

void increment_id();


#endif