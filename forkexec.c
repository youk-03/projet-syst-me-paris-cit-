
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#include "forkexec.h"



int forkexec(char * file_name, char ** arguments){
    int process_id=fork();
    if (process_id==-1){
        perror("forkexec: fork");
        exit (1);
    }
    if (process_id==0){
        errno=0;
        execvp(file_name,arguments);
        perror("forkexec: Incorrect command");
        return 1;
    } else {
        wait(NULL);
        return 0;
    }
}

