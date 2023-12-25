#include <stdio.h>
#include <stdlib.h>
#include "job.h"
#include "interrogation_exit.h"



int interrogation_point (int val){
    printf("%d\n", val);
    fflush(NULL);
    return 0;
}

int running_or_stopped (job_table* job_table){
    if (job_table == NULL){
        perror("table de job nulle\n");
        return 0;
    }
    for (int i=0; i<job_table->length; i++){
        job * job = job_table->table[i] ;
        if (job == NULL){
            perror("job null\n");
        } else {
            int p = job->status;
            if (p==1 || p==2 || p==-2){ //job.c l.129
                return 1;
            } 
        }
        
    }
    return 0;
}

int exit_jsh (int val, job_table* job_table){

    if (running_or_stopped(job_table)){
        fprintf(stderr,"Processus still in execution\n");
        fflush(NULL);
        return 1;
    }
    free_job_table(job_table);
    exit(val);
}
