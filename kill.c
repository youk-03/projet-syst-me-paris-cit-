#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <signal.h>
#include <string.h>



int kill_cmd(char * arg1, char * arg2){

    char * string_jobnumber=NULL;
    char * string_signumber=NULL;

if (arg2==NULL){ // example: kill %2 sends sigterm to all processes of job 2
    if (arg1[0]=='%'){
        string_jobnumber=malloc(strlen(arg1)); //
        if(string_jobnumber==NULL) {
            goto error ; 
        }
        memcpy(string_jobnumber,arg1+1,strlen(arg1)-1);
        // int sig=kill(atoi(string_jobnumber), SIGTERM); //this is not it, not functional for now
        // if (sig==-1) {
        //    goto error ;
        // }

        // free(string_jobnumber);

    } return 0;
    
} else {//example: kill -9 5312 sends SIGKILL (9) to process of pid 5312

    if(arg1[0]=='-'){

        string_signumber=malloc(strlen(arg1));
        if(string_signumber==NULL) {
            goto error ; 
        }
        memmove(string_signumber,arg1+1,strlen(arg1));
        int sig=kill(atoi(arg2),atoi(string_signumber));
         if (sig==-1) {
            goto error ;
         }

         free(string_signumber);

    return 0 ;
    } 

    error:
    if (string_jobnumber!=NULL){
        free(string_jobnumber);
    }
    if (string_signumber!=NULL){
        free(string_signumber);
    }
    perror ("kill_cmd");
    return 1;
    }
}


// int main(){
// kill_cmd("-9","28214"); //-9 sigkill, -19 sigstop, -18 sigcont (useful for tests)
// }