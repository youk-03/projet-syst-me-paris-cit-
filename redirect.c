#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "forkexec.h"
#include "mystring.h"
#include "redirect.h"
#include "pwd.h"

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

/*argument* pipe (arg, suite){
    int fd[2] = {-1,-1};
    if(pipe(fd)!=0){
        // erreur
    }

    process_id = fork
    if (process_id == -1){
        close(fd[0]);
        close(fd[1]);
        perror
    } if (process_id == 0) {
        close(fd[0]);
        dup2(fd[1],1);
        return arg
    } else {
        close(fd[1]);
        dup2(fd[0],0);
        waitpid(process_id);
        redirect(suite);
    }




    - renvoie un argument ?
    fonction récursive ou boucle ? 
    fork : comment on gère tous les processus et fait un sorte que chacun finisse ?
    Quand on execute une commande -> il faut passer par le main ?
    En plus des redirections, mettre les appels à forkexec dans une fonction
}*/

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