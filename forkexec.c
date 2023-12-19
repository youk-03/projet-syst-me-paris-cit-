
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include "forkexec.h"
#include "interrogation_exit.h"
#include "mystring.h"



int forkexec(char * file_name, char ** arguments){
    int process_id=fork();
    int status;
    if (process_id==-1){
        perror("forkexec: fork");
        exit (1);
    }
    if (process_id==0){
        errno=0;
        execvp(file_name,arguments);
        perror("forkexec: Incorrect command :");
        exit_jsh(1);
    } else {
        waitpid(process_id,&status,0);
        return WEXITSTATUS(status) ;
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
        exit_jsh(1);
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
