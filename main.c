#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include "mystring.h"
#include "cd.h"
#include "pwd.h"
#include "prompt.h"
#include "processus.h"
#include "interrogation_exit.h"
#include "forkexec.h"
#include "kill.h"
#include "redirect.h"
#include "jobs.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>


int main (int argc, char *argv[]){

    //copy of stdin stdout and stderr
    int stdout_ = dup(1);
    int stdin_ = dup(0);
    int stderr_ = dup(2);
    //

    rl_outstream = stderr;

    using_history();


    static int last_return = 0;
    static int job_number = 0;
    int pid = -1;
    char* line_read = NULL; //Readline
    char* prompt= NULL;
    argument *arg = NULL;
    char *path = NULL;
    processus_table* proc_table = allocate_processus_table(5);
    processus* proc;
    bool isredirect = false;

    //TODO : waitpid sur la table peut etre avoir un arg last status ? pour constater un changement
    //mettre a jour à chaque tour de boucle l'état des processus et appeler job si l'état d'un change
    //regarder dans man wait pour les fonc avec &status


    while(1){

    maj_process_table(proc_table,false);

    job_number= proc_table->length;
    //if(job_number == 0) id = 1; //to not have like a hundred id even when there is no more job
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

    
   // maj_process_table(proc_table);


    line_read = readline(prompt);

/////////////////////////////////////////////////////////////

    if(!line_read){ //ctrl+D
        last_return = exit_jsh(last_return,proc_table);
    }

    add_history (line_read);

    arg=split(line_read,' ');

    //CASE WHERE REDIRECT TO CHANGE THE FD AND CREATE THE FILE
    if(get_command(arg) == 5){
        argument* tmp = redirect(arg);
        if(tmp!=NULL){
            free_argument(arg);
            arg=tmp;       
        }
        else{
            last_return = 1;
        }
        isredirect= true;
    }
    switch(get_command(arg)){
        case 0: last_return = interrogation_point(last_return); break; //?
        case 1: //exit
        if(arg->nbr_arg > 1){
            last_return = exit_jsh(atoi(arg->data[1]),proc_table);
         }
        else{
            last_return = exit_jsh(last_return,proc_table);
        } 
        break; 
        case 2: last_return = pwd(); break; //pwd
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
        case 4: 

        if(is_background(arg)){

        pid = forkexecBackground(arg->data[0],arg->data); //forkexecBackground
        line_read[strlen(line_read)-2] = '\0';
        proc = allocate_processus(pid,getpid(),1,line_read/*,id++*/);

        if(proc != NULL) {

            if(setpgid(pid,0) != 0){
                perror("main l.132");
                }// verifier que succes
            add_processus(proc,proc_table);
            print_jobs(proc,2); //.... running ...
        } 
        else goto error;
        //creer proc ajouter a la table si pas erreur!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        }

        else{
        
        last_return = forkexec(arg->data[0],arg->data,proc_table);  //forkexec

        }

        break;

        case 6: 
        maj_process_table(proc_table,true);
        if (arg->nbr_arg > 1) {
            last_return = jobs(false,arg->data[1],proc_table);
        } else {
            last_return = jobs(false, NULL, proc_table);
        }
        break; //jobs

        case 7: //kill

        if(arg->nbr_arg > 2){
            last_return = kill_cmd(arg->data[1],arg->data[2],proc_table);
        }
        else{
            last_return = kill_cmd(arg->data[1],NULL,proc_table);
        }

        break;

        default: break;

    }


    //maj_main_print(proc_table);

    if(isredirect){
        
    //putting back every fd to normal

    dup2(stdout_,1); //////////////////
    dup2(stdin_,0); /////////////:
    dup2(stderr_,2); ////////////////////:
    isredirect=false;
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

  //free_processus_table(proc_table);

  error:
    exit_jsh(1,proc_table);


   

    return 0;
}