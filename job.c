#include "job.h"
#include "jobs_command.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
//job_table


//fonction return job qui prend char* type %1 et return le job a l'id correspondant

static int id = 1;

void increment_id(){
    id++;
}


job_table* allocate_job_table(size_t capacity){
    job_table* res = malloc(sizeof(job_table));
    if(!res) goto error;

    res->capacity = capacity;
    res->length = 0;
    res->table = malloc(sizeof(job*)*capacity);

    if(!res->table) goto error;

    return res;
    error:
    exit(1);
}

void free_job_table(job_table* table){

    for(int i=0; i<table->length; i++){
       free_job(table->table[i]);
    }

    free(table->table);
    table->table = NULL;
    free(table);
    table=NULL;

}

void add_job (job* job, job_table* table ){

    if(table->capacity == table->length){
        table->capacity = table->capacity*2;
        table->table = realloc(table->table, sizeof(job)*table->capacity);
        if(!table->table){
            goto error;
        
        }
    }

    table->table[table->length] = job; 
    table->length++;

    return;

    error:
    exit(1);

}

int delete_job (job* job, job_table* table){

    int i = 0;
    bool deleted = false;

    for(i; i<table->length; i++){
        if(table->table[i] == job){
            free_job(table->table[i]);
            deleted = true;
            break;
        }

    }

    if(deleted){
        memmove(table->table+i,table->table+i+1,(table->length-i-1)*sizeof(job));
        table->length--;
          if(table->length == 0){
        id = 1;
    }
    }
    else{
        return -1;
    }

    return 0;

}

job* get_job (job_table* job_table, char* id){

    if(id[0] != '%'){
        return NULL;
    }

    int job_id = atoi(&id[1]);

    for(int i=0; i<job_table->length; i++){
        if (job_table->table[i]->id == job_id){
            return job_table->table[i];
        }
    }

    return NULL;
}
//job

void free_job (job* job){

    free(job->name); 
    free(job);
    job=NULL;
}

job* allocate_job (pid_t job_pid, pid_t father_pid, int status, char* name, bool keep_id){
    job* res = malloc(sizeof(job));
    
    if(!res) goto error;

    res->job_pid = job_pid;
    res->father_pid = father_pid;
    res->status = status;
    res->name = malloc(strlen(name)+1); 
    res->name = strcpy(res->name,name);
    if(!res->name) goto error;
    res->id = id;
    if(keep_id){ //for forkexec where i create it juste to delete it in the end
    id++;
    }


    return res;

    error:
    exit(1);
}


void print_test (job_table* table){
    for(int i=0; i<table->length; i++){

        printf("je suis: %d, je m'appelle: %s\n", table->table[i]->job_pid, table->table[i]->name);

    }
}

int maj_job_table (job_table* job_table, bool stdout){
    int status = -1;
    int wait = -1;

    for(int i=0; i<job_table->length; i++){
       wait =  waitpid(job_table->table[i]->job_pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
       if(wait == 0){
          if(job_table->table[i]->status == 2){
           job_table->table[i]->status =-2; //so that it won't print each time "... STOPPED" 
        }
        continue;
       }
       if(wait == -1){
         if(errno == ECHILD){
            job_table->table[i]->status = 5; // 4 ?
        
        }
    else goto error;
       }

       else{
        

        if(WIFEXITED(status)){ //end with exit or return
            job_table->table[i]->status = 5; //done
        }
        else if(WIFSIGNALED(status)){ //terminated with signal
            job_table->table[i]->status = 4; //killed
        }
        else if(WIFSTOPPED(status)){
            job_table->table[i]->status = 2; //stopped
        }
        else if(WIFCONTINUED(status)){
            job_table->table[i]->status = 1; //continue
            print_jobs(job_table->table[i],2);
        }

       }
    }
    maj_main_print(job_table, stdout);
    return 0;

    error:
    perror("maj_jobs_table");
    return 1;
}

