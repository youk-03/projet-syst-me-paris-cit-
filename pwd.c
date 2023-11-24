#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>

#include "pwd.h"

/*Prints current working directory*/
int pwd(int nb_args){

    if(nb_args!=1){
        goto error ;
    }

    char pwd[PATH_MAX];
    if (getcwd(pwd,PATH_MAX)==NULL){
        perror("pwd : getcwd");
        goto error; 
    }
    int nb_written= write(STDOUT_FILENO,pwd,strlen(pwd));
    if (nb_written<strlen(pwd)) {
        perror("pwd : write");
        goto error; 
    }
    int line_break=write(STDOUT_FILENO,"\n",1);
    if (line_break!=1){
        perror("pwd : line break");
        goto error; 
    }
    return 0;

    error: 
    return 1 ;
}



