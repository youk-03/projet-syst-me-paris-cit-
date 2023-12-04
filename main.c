#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "mystring.h"
#include "cd.h"
#include "pwd.h"
#include "prompt.h"
#include "processus.h"
#include "interrogation_exit.h"
#include "forkexec.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>

int main (int argc, char *argv[]){
    rl_outstream = stderr;

    using_history();

    static int last_return = 0;
    static int job_number = 0;
    char* line_read = NULL; //Readline
    char* prompt= NULL;
    argument *arg = NULL;
    char *path = NULL;
    processus_table* proc_table = NULL;


    while(1){
    errno = 0;
    path = getcwd(NULL,0); 
    if(path == NULL){
        perror("Main - getcwd :");
        goto error;
    }
    prompt= create_prompt(path,job_number); 
    if(path == NULL){
        goto error;
    }
    line_read = readline(prompt);
    if(!line_read){ //ctrl+D
        exit_jsh(last_return);
    }

    add_history (line_read);

    arg=split(line_read,' ');
    switch(get_command(arg)){
        case 0: last_return = interrogation_point(last_return); break; //?
        case 1: //exit
        if(arg->nbr_arg > 1){
            exit_jsh(atoi(arg->data[1]));
         }
        else{
            exit_jsh(last_return);
        } 
        break; 
        case 2: last_return = pwd();break; //pwd
        case 3: 
            if(arg->nbr_arg == 1){ //case where it's only cd
                last_return = cd(0,NULL);
            }
            else if(arg->nbr_arg > 2){ //case where cd is incorrect arg like "cd dd sds z"
                last_return=1;
                char *error= "cd: too many arguments\n";
                if(write(STDERR_FILENO, error, strlen(error)) != strlen(error)){
                    perror("main: write ");
                }
                
            }

            else{//case for cd - or cd my/path
                last_return = cd(1,arg->data[1]);
            }

        break; //cd
        case 4: last_return = forkexec(arg->data[0],arg->data); break; //forkexec
        default: break;

    }
    if(arg != NULL){
      free_argument(arg);
    }
    if(line_read){ 
     free(line_read);
    line_read = NULL;
    }
    if(prompt != NULL){ 
     free(prompt);
    prompt = NULL;
    }
    if(path != NULL){ 
     free(path);
    path = NULL;
    }
}

  error:
    exit_jsh(1);


   

    return 0;
}
