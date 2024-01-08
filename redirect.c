#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "forkexec.h"
#include "mystring.h"
#include "redirect.h"
#include "pwd.h"
#include "job.h"
#include "prompt.h"
#include "interrogation_exit.h"
#include "cd.h"
#include "jobs_command.h"
#include "kill.h"
#include <signal.h>

static bool failure = false;

argument* redirect (argument* arg, int redirection[3]){

    errno=0;

    char* command = arg->data[0]; 
    int option = 0;
    int it = -1;
    int length = arg->nbr_arg;
    bool fic = false;
    char* fic_name = NULL;
    int fd_fic= -1;

    //count the number of option (to give to forkexec)

    for(int i=1; i<length; i++){ 
        if(return_redirect(arg->data[i]) == -1){ 
            option++;
        }
        else{
            break;
        }
    }

    it=option + 1;

    //browsing through the command (data) to dup2 and open properly
    while(it<length){
    switch(return_redirect(arg->data[it])) {

    case 0: //<

    fic = false;
    
    if(it+1<arg->nbr_arg){
    fd_fic = open(arg->data[it+1],O_RDONLY);
    if(fd_fic == -1){
        goto error;
    }

    dup2(fd_fic, 0);  

    }
    else{
        goto error;
    }
    redirection[0] = 1;

    break;


    case 1: //>

        fic = false;

        if(it+1<arg->nbr_arg){
            fd_fic = open(arg->data[it+1],O_CREAT|O_EXCL|O_WRONLY, 0644); 
            if(fd_fic == -1){
                goto error;
            }   
            dup2(fd_fic, 1);     
         
        }
        else{
            goto error; 
        }
         redirection[1] = 1;
        break;
    case 2: //>>

        fic = false;

        if(it+1<arg->nbr_arg){
            fd_fic = open(arg->data[it+1],O_CREAT|O_APPEND|O_WRONLY, 0644); 
            if(fd_fic == -1){
                goto error;
            }    

            dup2(fd_fic, 1); 
        }
        else{
            goto error; 
        }
        redirection[1] = 1;
    break;
    case 3://>|

        fic = false;

         if(it+1<arg->nbr_arg){
            fd_fic = open(arg->data[it+1],O_CREAT|O_TRUNC|O_WRONLY, 0644); 
            if(fd_fic == -1){
                goto error;
            }    

            dup2(fd_fic, 1); 
        }
        else{
            goto error; 
        }
        redirection[1] = 1;
        break;
    case 4://2>

        fic = false;

         if(it+1<arg->nbr_arg){
            fd_fic = open(arg->data[it+1],O_CREAT|O_EXCL|O_WRONLY, 0644); 
            if(fd_fic == -1){
                goto error;
            }    

            dup2(fd_fic, 2); 
        }
        else{
            goto error; 
        }
        redirection[2] = 1;
        break;
    case 5: //2>>

        fic = false;

             if(it+1<arg->nbr_arg){
            fd_fic = open(arg->data[it+1],O_CREAT|O_APPEND|O_WRONLY, 0644); 
            if(fd_fic == -1){
                goto error;
            }    

            dup2(fd_fic, 2); 
        }
        else{
            goto error; 
        }
        redirection[2] = 1;
        break;
    case 6: //2>|

        fic = false;

             if(it+1<arg->nbr_arg){
            fd_fic = open(arg->data[it+1],O_CREAT|O_TRUNC|O_WRONLY, 0644); 
            if(fd_fic == -1){
                goto error;
            }    

            dup2(fd_fic, 2); 
        }
        else{
            goto error; 
        }
        redirection[2] = 1;
        break;

    default: if(fic) {goto error;} fic=true; break; 



    }
    it++;

    }

    argument* res= cpy_argument(arg,option+1);

    return res;

    error:
    if(errno != 0){
        perror("redirect");
        errno = 0;
    }
    else{
     char* msg= "incorrect command line\n";   
     write(2, msg, strlen(msg));
    }

    return NULL;

}

int return_redirect(char * string){

    // < 0
    // > 1
    // >> 2
    // >| 3
    // 2> 4
    // 2>> 5
    // 2>| 6
    // -1

    if(strcmp(string, "<") == 0) return 0;

    if(strcmp(string, ">") == 0) return 1;

    if(strcmp(string, ">>") == 0) return 2;

    if(strcmp(string, ">|") == 0) return 3;

    if(strcmp(string, "2>") == 0) return 4;

    if(strcmp(string, "2>>") == 0) return 5;

    if(strcmp(string, "2>|") == 0) return 6;

    return -1;
}

int exec_command (argument* arg, job_table* job_table, int last_return, int shell_pgid, int shell_fd, int redirection[3], job* job){
    int ret = 1;

    //faut peut être les fermer non ?
    int stdout_ = dup(1);
    int stdin_ = dup(0);
    int stderr_ = dup(2);


    bool isredirect = false;

       if(get_command(arg) == 5){
        argument* tmp = redirect(arg, redirection);
        if(tmp!=NULL){
            free_argument(arg);
            arg=tmp;       
        }
        else{
            failure = true;
             if(redirection[1] == 1){
            dup2(stdout_,1); 
            redirection[1] = 0;
            }
            if(redirection[0] == 1){
            dup2(stdin_,0); 
            redirection[0] == 0;
            }
            if(redirection[2] == 1){
            dup2(stderr_,2);
            redirection[2] == 0; 
        }
            return 1; 
        }
        isredirect= true;
    }


    switch(get_command(arg)){
        case 0: ret = interrogation_point(last_return); break; //?
        case 1: //exit
        if(arg->nbr_arg > 1){
            ret = exit_jsh(atoi(arg->data[1]),job_table);
         }
        else{
            ret = exit_jsh(last_return,job_table);
        } 
        break; 
        case 2: ret = pwd(); break; //pwd
        case 3: 
            if(arg->nbr_arg == 1){ //case where it's only cd
                ret = cd(0,NULL);
            }
            else if(arg->nbr_arg > 2){ //case where cd is incorrect arg like "cd dd sds z"
                ret=1;
                char *error= "cd: too many arguments\n";
                if(write(STDERR_FILENO, error, strlen(error)) != strlen(error)){
                    perror("main: write ");
                    errno = 0;
                }
                
            }

            else{//case for cd - or cd my/path
                ret = cd(1,arg->data[1]);
            }

        break; //cd
         case 4: 
        execvp(arg->data[0],arg->data);
        perror("exec: Incorrect command :");
        exit(1); 

        break;

        case 6: 
        maj_job_table(job_table,true);
        if (arg->nbr_arg > 1) {
            ret = jobs(false,arg->data[1],job_table);
        } else {
            ret = jobs(false, NULL, job_table);
        }
        break; //jobs

        case 7: //kill

        if(arg->nbr_arg > 2){
            ret = kill_cmd(arg->data[1],arg->data[2],job_table);
        }
        else{
            ret = kill_cmd(arg->data[1],NULL,job_table);
        }

        break;

        default: break; //redirect echoue 

    }
    free_argument(arg); ///////////////////////////

        if(isredirect){
        
    //putting back every fd to normal
    if(redirection[1] == 1){
    dup2(stdout_,1); 
    redirection[1] = 0;
    }
    if(redirection[0] == 1){
    dup2(stdin_,0); 
    redirection[0] == 0;
    }
    if(redirection[2] == 1){
    dup2(stderr_,2);
    redirection[2] == 0; 
    }
    isredirect=false;
    }

    return ret;
}

int is_return_redirect (argument* arg){
    int res = -1;
     for(int i=0; i<arg->nbr_arg; i++){
        res = return_redirect(arg->data[i]);
      if(res != -1){
        return res;
      }
   }
   return -1; 
}

int mpipe (char* line, job_table* job_table, int last_return, int shell_pgid, int redirection[3], job* job){
    argument* arg = split(line,'|'); 
    int ret = 1;

    int stdout_ = dup(1);
    int stdin_ = dup(0);
    int stderr_ = dup(2);


    int pipefd_old[2] = {-1,-1}; //0 lecture 1 ecriture
    int pipefd_new[2] = {-1,-1};
    int length = arg->nbr_arg;
    int pid = -1;
    int redirect_res = -1;


    for(int i=0; i<length; i++){
        if(i < length){
            if(pipe(pipefd_new)!= 0) goto error; 
        }
        pid = fork();
        switch(pid){
            case -1: 
                close(pipefd_new[0]);
                close(pipefd_new[1]);
                goto error;
                break; //erreur
            case 0: //child
                argument* arg2 = split(arg->data[i],' ');
                exec(job,stdin_);
                
                if(i == 0){ //case where < redirect is present 0 if is there else -1
           
                    redirect_res = is_return_redirect(arg2); 
                    if(redirect_res != -1 && redirect_res != 0){
                
                    write(stderr_, "pipeline: incorrect redirect\n", 29); 
                    free_argument(arg2);
                    arg2 = NULL;
                    close(pipefd_new[0]);
                    close(pipefd_new[1]);
                    failure = true;
                    goto error;
                    }
                }


                if(i > 0){
                    if(i != length-1){
                        redirect_res = is_return_redirect(arg2); 
                        if(redirect_res != -1 && redirect_res != 4 && redirect_res != 5 && redirect_res != 6){
                      
                        write(stderr_, "pipeline: incorrect redirect\n", 29);
                        free_argument(arg2);
                        arg2 = NULL;
                        close(pipefd_new[0]);
                        close(pipefd_new[1]);
                        failure = true;
                        goto error;
                
                        }
                    }

                    if(dup2(pipefd_old[0], 0)== -1) perror("dup2");
                    close(pipefd_old[0]);
                    close(pipefd_old[1]);
                }
                if(i<length-1){
                    close(pipefd_new[0]);
                    if(dup2(pipefd_new[1], 1) == -1) perror("dup2");       
                    close(pipefd_new[1]);
                } 
                if(i == length - 1){
                    redirect_res = is_return_redirect(arg2); 
                    if(redirect_res != -1 && redirect_res != 1 && redirect_res != 2 && redirect_res != 3){
             
                    write(stderr_, "pipeline: incorrect redirect\n", 29); 
                    free_argument(arg2);
                    arg2 = NULL;
                    failure = true;
                    goto error;
                    }
                }

                ret = exec_command(arg2, job_table, last_return, shell_pgid, stdin_, redirection, job);
           
                if(failure){
                    goto error;
                }

                exit(ret);
            default: 
            if(i==0){
                 job->job_pid = pid;
                 job->status = 1;
            }

            //remettre les redirection a la normal ?

                if(i>0){
                    close(pipefd_old[0]);
                    close(pipefd_old[1]);       
                }
                if(i<length){
                    pipefd_old[0]=pipefd_new[0];
                    pipefd_old[1]=pipefd_new[1];
                }
            
            break; //pere
        }
    }

    int status = -1;
    bool redirect_prb = false;
    bool is_stopped = false;
    for(int i = 0 ; i<length; i++){ 
         if(redirect_prb){
            if (kill(-(job->job_pid),9) == -1 ){//so that if there is a redirection problem everyone die
                perror("kill pipeline:");
            }
        }
        if(is_stopped){
            if (kill(-(job->job_pid),20) == -1 ){//if one is stopped they al stop (not really like that with pipeline but idk how to do)
                perror("stop pipeline:");
            }
        }
        waitpid(-(job->job_pid),&status,WUNTRACED); //apparemment c'est suffisant pour que ça marche ???
       
        if(WIFSIGNALED(status)){
            redirect_prb = true;
        }
        if(WIFSTOPPED(status)){
            is_stopped = true;
            job->status = 2; //stopped
        }
    }

    if(redirect_prb){
       job->status = 4; //killed 
    }
    if(!is_stopped){
    job->status = 5;//done    
    delete_job(job, job_table);
    }

    close(pipefd_old[0]);
    close(pipefd_old[1]);
    if(arg != NULL){
    free_argument(arg);
     arg = NULL;
    }
    dup2(stdout_,1); 
    dup2(stdin_,0); 
    dup2(stderr_,2); 
    close(stdout_);
    close(stdin_);
    close(stderr_);
    return 0;


    error:
    // if(arg != NULL){
    // // free_argument(arg);
    // // arg = NULL;
    // }
    dup2(stdout_,1); 
    dup2(stdin_,0); 
    dup2(stderr_,2); 
    if(errno != 0){
    perror("pipe : ");
    errno = 0;
    }
    close(stdout_);
    close(stdin_);
    close(stderr_);
    if(failure){ 
        failure = false;
        kill(getpid(),9); //killing the child when error
        
        }
    
    return 1;


}

/*int mypipe (const char* line, job_table* job_table, int last_return){ //cas dup départ pas fait
    argument* arg = split(line,'|');

    int ret = 1;
    int stdout_ = dup(1);
    int stdin_ = dup(0);
    int stderr_ = dup(2);

    for (int i=0; i<arg->nbr_arg-1; i++){
        int fd[2] = {-1,-1};

        if(pipe(fd)!=0){
            goto error;
        }

        int process_id = fork();
        if (process_id == -1){
            close(fd[0]);
            close(fd[1]);
            goto error;
        } if (process_id == 0) { // fils
            close(fd[0]); // ferme lecture
            dup2(fd[1],1);
            argument* arg2 = split(arg->data[i],' ');
            ret = exec_command(arg, job_table, last_return);
            free_argument(arg2);
            exit(0);

        } else { // père
            close(fd[1]);
            dup2(fd[0],0);
            close(fd[0]);
            if( waitpid(process_id, NULL, 1) == -1){
                goto error;
            }

        }

        if (i==arg->nbr_arg-1){ //fin du pipe
            argument* arg2 = split(arg->data[i+1],' ');
            ret = exec_command(arg, job_table, last_return);
            free_argument(arg2);
            close(fd[0]);
        }
    }

    free_argument(arg);
    dup2(stdout_,1); 
    dup2(stdin_,0); 
    dup2(stderr_,2); 

    return ret;


    error :
        free_argument(arg);
        dup2(stdout_,1); 
        dup2(stdin_,0); 
        dup2(stderr_,2); 
        perror("pipe : ");
        // rétablir fd
        return 1;
}*/


    /*
    - renvoie un argument ?
    fonction récursive ou boucle ? 
    fork : comment on gère tous les processus et fait un sorte que chacun finisse ?
    Quand on execute une commande -> il faut passer par le main ?
    En plus des redirections, mettre les appels à forkexec dans une fonction
    */

// int main (int argc, char** argv){

//     //int mpipe (char* line, job_table* job_table, int last_return, int shell_pgid)

//     mpipe("ls | sleep",)

//     return 0;
// }