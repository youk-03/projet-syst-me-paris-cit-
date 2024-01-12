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

void free_job (job* job){ //deja modif pour pipeline en fait

    if(job->process_number > 0){
        for(int i=0; i<job->process_number; i++){
            free_job(job->process_table[i]); 
        }
    }
    if(job->process_table != NULL){
    free(job->process_table);
    }
    free(job->name); 
    free(job);
    job=NULL;
}

job* allocate_job (pid_t job_pid, pid_t father_pid, int status, char* name, bool keep_id){
    job* res = malloc(sizeof(job));
    
    if(!res) goto error;

    res->job_pid = job_pid;
    res->process_number = 0;
    res->process_table = NULL;
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

bool is_stopped(job* job){
   
    for(int i=0; i<job->process_number; i++){
        if(job->process_table[i]->status == 2){
            return true;
        }
    }
    return false;
}

bool is_killed_or_done(job* job, int status){
   // printf("job process_number = %d\n", job->process_number);
    int cpt = 0;
    for (int i=0; i<job->process_number; i++){
        if(job->process_table[i]->status == status){
            cpt++;
        }
    }
    return cpt == job->process_number;
}

int delete_from_process_table(job* jobs, job* job_to_delete){
    if(jobs->process_number == 0){
        dprintf(2,"delete_from_process_table: empty process_table");
        return 1;
    }

    for(int i=0; i<jobs->process_number; i++){
        if(jobs->process_table[i] == job_to_delete){
            free_job(jobs->process_table[i]);
            memmove(jobs->process_table+i,jobs->process_table+i+1,sizeof(job)*(jobs->process_number-(i-1)));
            jobs->process_number--;
            return 0;
        }

        
    }
    return 1;
}

int delete_killed_process(job* jobs){
    int i=0;
    while(i<jobs->process_number){
        if(jobs->process_table[i]->status == 4){
            delete_from_process_table(jobs,jobs->process_table[i]);
        }
        i++;
    }

    return 0;
}

//0 1 2 3 4 5
//number_process = 6
//met status a jour, j'appelle delete_killed_processus sur le machin
//si pipeline etais a run et qu'ils sont tous à stop le job -> stop !!
//pareil pour kill !!

int maj_process_table(job* job){

    bool killed = false;
    bool done = false;
    if(job->process_number < 1){
        dprintf(2,"maj_process_table job empty");
        return 1;
    }
    int wait = -1;
    int status = -1;
    int i=0;
    while(i<job->process_number){
        wait = waitpid(job->process_table[i]->job_pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
        if(wait == 0){
            //rien
            continue;
        }
        else if(wait == -1){
             if(errno == ECHILD){
            job->process_table[i]->status = 4; 
            }
            else {
             goto error;
            }
        }
        
       else{
        

        if(WIFEXITED(status)){ //end with exit or return
            job->process_table[i]->status = 5; //done
            delete_from_process_table(job, job->process_table[i]); //pas i++
            done = true;
        }
        else if(WIFSIGNALED(status)){ //terminated with signal
            job->process_table[i]->status = 4; //killed
            delete_from_process_table(job, job->process_table[i]); 
            killed = true;
        }
        else if(WIFSTOPPED(status)){
            job->process_table[i]->status = 2; //stopped
            i++;
        }
        else if(WIFCONTINUED(status)){
            job->process_table[i]->status = 1; //continue
            i++;
        }

       }
    }

    if(job->process_number == 0 && killed){
        //killed
        job->status = 4;
    }
    else if(job->process_number == 0 && done){
        job->status = 5;
        //done
    }
    else if(is_killed_or_done(job,1)){
        job->status = 1;
        //running
    }
    else if(is_stopped(job)){
        if(job->status = 2){
            job->status = -2;
        }
        else {
        job->status = 2;
        }
        //stopped
    }


    return 0;
    error:
    perror("maj_process_table");
    return 1;

}

int maj_job_table (job_table* job_table, bool stdout){ //fonction intermediaire qui wait sur la process_table ?
    int status = -1;
    int wait = -1;

    for(int i=0; i<job_table->length; i++){
       if(job_table->table[i]->process_number>0){
        maj_process_table(job_table->table[i]);
        continue; //bc already maj in maj_process table
       } 
       wait =  waitpid(job_table->table[i]->job_pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
       if(wait == 0){
          if(job_table->table[i]->status == 2){
           job_table->table[i]->status =-2; //so that it won't print each time "... STOPPED" 
        }
        continue;
       }
       if(wait == -1){
         if(errno == ECHILD){
            job_table->table[i]->status = 4; 
        
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

int allocate_process_table(job* job){

    job->process_table = malloc(sizeof(job)*MAX_PIPE);
    if(job->process_table == NULL){
        exit(1);
    }
    return 0;
}

