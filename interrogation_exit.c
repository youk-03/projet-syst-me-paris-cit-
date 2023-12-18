#include <stdio.h>
#include <stdlib.h>
#include "processus.h"
#include "interrogation_exit.h"



int interrogation_point (int val){
    printf("%d\n", val);
    return 0;
}

int running_or_stopped (processus_table* proc_table){
    if (proc_table == NULL){
        perror("table de processus nulle\n");
        return 0;
    }
    for (int i=0; i<proc_table->length; i++){
        processus * proc = proc_table->table[i] ;
        if (proc == NULL){
            perror("processus null\n");
        } else {
            int p = proc->status;
            if (p==1 || p==2){
                return 1;
            } 
        }
        
    }
    return 0;
}

int exit_jsh (int val, processus_table* proc_table){
    if (running_or_stopped(proc_table)){
        fprintf(stderr,"Processus still in execution\n");
        return 1;
    }
    exit(val);
}

/*void exit_jsh (int val){
    exit(val);
}*/