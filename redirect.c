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

int redirect (argument* arg){

    errno=0;

    int stdout_ = dup(1);
    int stdin_ = dup(0);
    int stderr_ = dup(2);

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
            fd_fic = open(arg->data[it+1],O_CREAT|O_EXCL|O_WRONLY, 0766); 
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
            fd_fic = open(arg->data[it+1],O_CREAT|O_APPEND|O_WRONLY, 0766); 
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
            fd_fic = open(arg->data[it+1],O_CREAT|O_TRUNC|O_WRONLY, 0766); 
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
            fd_fic = open(arg->data[it+1],O_CREAT|O_EXCL|O_WRONLY, 0766); 
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
            fd_fic = open(arg->data[it+1],O_CREAT|O_APPEND|O_WRONLY, 0766); 
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
            fd_fic = open(arg->data[it+1],O_CREAT|O_TRUNC|O_WRONLY, 0766); 
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

    char** commands = malloc(sizeof(char*)*(option+2));

    int i=0;
    while(i<option+1){
        commands[i] = arg->data[i];
        i++;
    }

    commands[i] = NULL; //exec

    int res = forkexec(arg->data[0],commands);

    //putting back every fd to normal

    dup2(stdout_,1);
    dup2(stdin_,0);
    dup2(stderr_,2);

    free(commands);

    return res;  
    error:
    if(errno != 0){
        perror("redirect");
    }
    else{
     char* msg= "incorrect command line\n";   
     write(2, msg, strlen(msg));
    }

    dup2(stdout_,1);
    dup2(stdin_,0);
    dup2(stderr_,2);

    return 1;

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