#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <signal.h>
#include <string.h>

#include "job.h"


//%2   //-4  //arg2 -> NULL sauf si pid
int kill_cmd(char * arg1, char * arg2, job_table * tab){

    char * string_jobnumber=NULL;
    char * string_signumber=NULL;

    if(arg1 == NULL){
        goto error;
    }


if (arg2==NULL){ // example: kill %2 sends sigterm to all processes of job 2

    if (arg1[0]=='%'){
        string_jobnumber=calloc(strlen(arg1),'0'); 
        if(string_jobnumber==NULL) {
            perror("malloc");
            exit(1);
        }
        memcpy(string_jobnumber,arg1+1,strlen(arg1)-1);

     

        for (int i=0; i<tab->length; i++){

            if(tab->table[i]->id==atoi(string_jobnumber)){
                int sig=kill(-tab->table[i]->job_pid,SIGTERM); 
                if(sig==-1){
                    goto error ;
                }

                 break;
    
            }
        }

        free(string_jobnumber);
        return 0;

    } else {
        
        int sig=kill(atoi(arg1),SIGTERM); 
                if(sig==-1){
                    goto error ;
                }
        return 0;
    }
    
}



 else {//example: kill -9 5312 sends SIGKILL (9) to job of pid 5312


    if(arg1[0]=='-'){

        if(arg2[0] == '%'){

        string_jobnumber=malloc(strlen(arg2));  
        if(string_jobnumber==NULL) {
            perror("malloc");
            exit(1);
        }
        memcpy(string_jobnumber,arg2+1,strlen(arg2)); 

        //copy job number 

        string_signumber=malloc(strlen(arg1));

        if(string_signumber==NULL) {
            perror("malloc");
            exit(1);
        }
        
        memmove(string_signumber,arg1+1,strlen(arg1));

        //copy signumber
     

        for (int i=0; i<tab->length; i++){

            if(tab->table[i]->id==atoi(string_jobnumber)){
                int sig=kill(-tab->table[i]->job_pid,atoi(string_signumber)); 
                if(sig==-1){
                    goto error ;
                }
    
            }
        }

        free(string_jobnumber);
        free(string_signumber);


        }
        else{
        string_signumber=malloc(strlen(arg1));

        if(string_signumber==NULL) {
            perror("malloc");
            exit(1);
        }
        
        memmove(string_signumber,arg1+1,strlen(arg1));
        int sig=kill(atoi(arg2),atoi(string_signumber));
         if (sig==-1) {
            goto error ;
         }

         free(string_signumber);
        }

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