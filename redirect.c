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

argument* redirect (argument* arg){

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

int exec_command (argument* arg, job_table* job_table, int last_return){
    int ret = 1;
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
                }
                
            }

            else{//case for cd - or cd my/path
                ret = cd(1,arg->data[1]);
            }

        break; //cd
        case 4: 

        ret = forkexec(arg->data[0],arg->data,job_table);  //forkexec

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

        default: break;

    }
    return ret;
}

int mypipe (const char* line, job_table* job_table, int last_return){
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
            ret = exec_command(arg2, job_table, last_return);
            free_argument(arg2);
            exit(0);

        } else { // père
            close(fd[1]);
            dup2(fd[0],0);
            close(fd[0]);
            waitpid(process_id, NULL, 1);

        }

        if (i==arg->nbr_arg-1){ //fin du pipe
            argument* arg2 = split(arg->data[i+1],' ');
            ret = exec_command(arg2, job_table, last_return);
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
}

argument* process_substitution(const char* line, job_table* job_table, int last_return) {
    argument* arg = split(line, '<');
    int fl[arg->nbr_arg-1];

    for (int i=0; i<arg->nbr_arg-1; i++){

        int tube[2] = {-1, -1};
        if(pipe(tube)!=0){
            goto error;      
        }

        int process_id = fork();
        if (process_id==-1){
            goto error;
        }
        if (process_id==0){ // fils
            close(tube[0]);

            if (dup2(tube[1],1)==-1) {perror("echec dup2 ----\n");}
            else { printf(" Succes dup2 \n");}
            
            char* s = arg->data[i+1];
            for (int j=0; j<strlen(s); j++){
                if (s[j]=='('|| s[j]==')') s[j]=' '; // un peu dangereux ???? ///////////////
            }
            printf("%s \n", s);
            argument* arg2 = split(s, ' ');
            exec_command(arg2, job_table, last_return);
            free_argument(arg2);
            exit(0);
        } else { // père
            close(tube[1]);
            fl[i] = tube[0];
            wait(NULL);
        }
    }

    char * cmd = arg->data[0];
    for (int i=0; i<arg->nbr_arg-1; i++){
        sprintf(cmd, "%s %i", cmd, fl[i]);
    }
    argument* arg3 = split(cmd, ' ');
    return arg3;
    for (int i=0; i<arg->nbr_arg-1; i++){
        close(fl[i]);
    }

    error :

    free_argument(arg);
    perror("process substitution : ");
    return NULL;

}

/*
[0]...x/brunetj/Documents/SY$ diff <( ls projet-sy5) <( ls sy5-2023-2024)
forkexec: Incorrect command :: No such file or directory
forkexec: Incorrect command :: No such file or directory
malloc(): memory corruption (fast)

diff <( ls projet-sy5 ) <( ls sy5-2023-2024 )
forkexec: Incorrect command :: No such file or directory
forkexec: Incorrect command :: No such file or directory
diff: 0x7fffe7c0aa10: Aucun fichier ou dossier de ce type
diff: 0x7fffe7c0aa10: Aucun fichier ou dossier de ce type
corrupted size vs. prev_size

faire des malloc

*/


// int main (int argc, char** argv){

//     //>|     
//     // int fd_fic = open(argv[1], O_CREAT|O_TRUNC|O_WRONLY, 0700); 
//     // if(fd_fic == -1){
//     //     perror("main");
//     // }   
//     // dup2(fd_fic, STDOUT_FILENO);  ///////////////////////////


//     // char* ex[] = {"ls", NULL};
//     // execvp(ex[0], ex);    

//     char* tosplit = "cat -b < TEST > i ";
    
//     argument* arg = split(tosplit, ' ');
//     int test= redirect(arg);
//     printf("%d\n",test);




//     free_argument(arg);
         
    
// return 0;


// }