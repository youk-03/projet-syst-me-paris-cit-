#include <stdio.h>
#include <errno.h>
#include <string.h>
#include<unistd.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "mystring.h"
#include "redirect.h"
#include "prompt.h"
#include <stdbool.h>

char* truncate_to_size (char* path, int max_size){
   if(path == NULL){
      return NULL;
   }
   char *res = malloc(max_size+1); 
   if(!res){
      return NULL;
   }
   memmove(res,"...",3); 
   memmove(res+3,path+(strlen(path)-(max_size-3)),max_size-2);
   return res;
} //need to free char* returned by fonc + if path == NULL return NULL
//trunc path from left

char* create_prompt (char *path, int job_number){

bool istrunc = false;
size_t s_path = strlen(path);
int job_number_size = digit_number(job_number);
int ic=0;

if(s_path > 26-job_number_size){

   int max_size = 26-job_number_size;

   path = truncate_to_size(path,max_size);

   if(path == NULL){
      goto error;
   }
   s_path=26-job_number_size;
   istrunc = true;

}
size_t length = 25+job_number_size+s_path+1; //25= size of needed char for everything other than path and job_number

char *res = malloc(length); 
if(!res){
   goto error;
}
ic = strlen("\001\033[33m\002[");
memmove(res,"\001\033[33m\002[",ic);
sprintf(res+ic,"%d",job_number);
ic+=job_number_size;
char *res2 = "]\001\033[35m\002"; 
memmove(res+ic,res2,strlen(res2));
ic+= strlen(res2);
memmove(res+ic,path,s_path);
ic+=s_path;
char *res3 = "\001\033[00m\002$ ";
memmove(res+ic, res3,strlen(res3));
ic+=strlen(res3);
memmove(res+ic,"\0",1);

if(istrunc){ 
if(path){
free(path);
path=NULL;
}
}
 return res;

error: 
perror("malloc create_prompt");
return NULL;
} //need to free res*

int digit_number (int number){
   if(number == 0){
      return 1;
   }
   int count= 0;
   while(number != 0){
      number/=10;
      count++;
   }
   return count;
}

bool is_redirect(argument* arg){

   for(int i=0; i<arg->nbr_arg; i++){
      if(return_redirect(arg->data[i]) != -1) return true;
   }

   return false;
}

bool is_pipe(argument* arg){ //////////////////////////////////////////////////////////////////////////////////////
  for(int i=0; i<arg->nbr_arg; i++){
      if(strcmp(arg->data[i], "|") == 0) return true;
   }
   return false; 
}

int get_command(argument* arg){
   if(arg == NULL){
      return -1;
   }

   char *command = arg->data[0];

   //0 -> ?
   //1 -> exit
   //2 -> pwd
   //3 -> cd
   //4 exec or invalid argument
   //5 redirect
   //6 jobs
   //7 kill
   //8 bg
   //9 fg
   if(is_redirect(arg)){
      return 5;
   }
   
   if(strcmp(command,"?") == 0){
      return 0;
   }
   if(strcmp(command,"exit") == 0){
      return 1;
   }
   if(strcmp(command,"pwd") == 0){
      return 2;
   }
   if(strcmp(command,"cd") == 0){
      return 3;
   }


   if(strcmp(command,"jobs") == 0){
      return 6;
   }

     if(strcmp(command,"kill") == 0){
      return 7;
   }

   if(strcmp(command,"bg") == 0){
      return 8;
   }

   if(strcmp(command,"fg") == 0){
      return 9;
   }
   return 4;


}

