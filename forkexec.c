
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "forkexec.h"
#include "interrogation_exit.h"
#include "mystring.h"
#include "job.h"
#include "jobs_command.h"

//credit to https://www.gnu.org/software/libc/manual/html_node/Foreground-and-Background.html
// to understand how foreground job and background job work

void reset_signal(int act){ //SIG_DFL or SIG_IGN
    if(act != 1 && act != 0){
        return;
    }
    struct sigaction action = {0};
    if(act == 0){
        action.sa_handler = SIG_DFL;
    }
    else{
        action.sa_handler = SIG_IGN;
    }
    int signal_to_dfl[] = {SIGINT,SIGTERM,SIGTTIN,SIGQUIT,SIGTTOU,SIGTSTP};

    for(int i=0; i<6; i++){
        if(sigaction(signal_to_dfl[i], &action, NULL) == -1){
            perror("sigaction");
            exit_jsh(1,NULL);
        }
    }
}






int forkexec(char * file_name, char ** arguments, job_table* job_table){
    errno=0;
    int process_id=fork();
    int status;
    int wait = -1;
    if (process_id==-1){
        perror("forkexec: fork");
        exit (1);
    }
    if (process_id==0){

        if(setpgid(getpid(),getpid()) != 0){
                perror("forkexec l.30");
            }
        
        put_foreground(NULL, getpgid(getpid()), job_table); //////////////////////////////////////////////////:

        reset_signal(0);

        execvp(file_name,arguments);
        perror("forkexec: Incorrect command :");
        exit(1); //TODO : changer
    } else {

         job* job = allocate_job(process_id, getppid(), 1, file_name, false); 
         add_job(job, job_table);

         int res = wait_for_job(job,job_table,true);

         return res;
        ///////////////////////////////////////////////////////////////
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
         reset_signal(0);
         if(setpgid(getpid(),getpid()) != 0){
                perror("forkexec l.97");
                }

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

int put_foreground (job *job, pid_t pgid, job_table* job_table){
    bool cont = false;


    if(job == NULL && pgid != -1){

        if(tcsetpgrp(STDIN_FILENO, pgid) == -1){
        perror("put foreground");
        return 1;
    }
    }
    else {

    if(job == NULL){
        dprintf(2,"fg: invalid id\n");
        return 1;
    }

      if(job->status == 4){
        cont = true;
    }

    if(tcsetpgrp(STDIN_FILENO, job->job_pid) == -1){
        perror("put foreground");
        return 1;
    }

    if(cont){ 

        if(kill(-(job->job_pid), SIGCONT)<0){
        perror("put_foreground");
        return 1;
    }

    }

    wait_for_job(job, job_table,false);

    //restoring the shell is done after the function call
    }

    return 0;
}

void put_jsh_foreground (pid_t shell_pgid){

    if(tcsetpgrp(STDIN_FILENO, shell_pgid) == -1){
        perror("put_jsh_foreground");
    }
}

int put_background (job *job){
    if (job == NULL){
        return 1;
    }
    if(kill(-job->job_pid, SIGCONT)<0){
        perror("put_background");
        return 1;
    }
    return 0;
}


int wait_for_job (job *job, job_table *job_table, bool id){ //delete de la job table !!

    int status = -1;

    int wait = waitpid(job->job_pid,&status,WUNTRACED);
       if(wait == -1){
        perror("forkexec");
        return 1;
       }
       if(wait > 0) {
        if(WIFSIGNALED(status)){

            job->status = 4;
            print_jobs(job,2);
            delete_job(job, job_table);

            return 1;

        }
        else if (WIFSTOPPED(status)){

            job->status = -2;
            if (id) {
                increment_id();
            }
            print_jobs(job,2);

            return 1;

        }
        else if(WIFEXITED(status)){
        delete_job(job, job_table);
        return WEXITSTATUS(status);
        }
       }
    

}

