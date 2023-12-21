
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "forkexec.h"
#include "interrogation_exit.h"
#include "mystring.h"
#include "processus.h"
#include "jobs.h"



int forkexec(char * file_name, char ** arguments, processus_table* proc_table){
    int process_id=fork();
    int status;
    int wait = -1;
    if (process_id==-1){
        perror("forkexec: fork");
        exit (1);
    }
    if (process_id==0){
        errno=0;
        execvp(file_name,arguments);
        perror("forkexec: Incorrect command :");
        exit(1); //TODO : changer
    } else {
       wait = waitpid(process_id,&status,WUNTRACED);
       if(wait == -1){
        perror("forkexec");
        return 1;
       }
       if(wait > 0) {
        if(WIFSIGNALED(status)){

            processus* proc = allocate_processus(process_id,getppid(), 4, file_name);
            print_jobs(proc,2);
            free_processus(proc);

            return 1;

        }
        else if (WIFSTOPPED(status)){

            processus* proc = allocate_processus(process_id,getppid(), -2, file_name);
            add_processus(proc, proc_table);
            print_jobs(proc,2);

            return 1;

        }
        else if(WIFEXITED(status)){
        return WEXITSTATUS(status);
        }
       }
    }
}

int forkexecBackground( char * file_name, char ** arguments){
    int process_id=fork();
    int status;
    if (process_id==-1){
        perror("forkexecBackground : fork");
        exit (1);
    }
    if (process_id==0){
        errno=0;
        execvp(file_name,arguments);
        perror("forkexecBackground : Incorrect command :");
        exit(1); //TODO: changer
    } else {
        return process_id ;
    }
}

bool is_background(argument* arg){

    if(strcmp(arg->data[arg->nbr_arg-1], "&") == 0){
        free(arg->data[arg->nbr_arg-1]);
        arg->data[arg->nbr_arg-1] = NULL;
        arg->nbr_arg--;

        return true;
    }

    return false;

}
