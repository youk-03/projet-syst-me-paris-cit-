#include <stdio.h>
#include <errno.h>
#include <string.h>
#include<unistd.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "mystring.h"
#include <stdbool.h>
#define PROMPT_CHAR_MAX 25

char* truncate_to_size (char* path){
   if(path == NULL){
      return NULL;
   }
   char *res = malloc(PROMPT_CHAR_MAX+1); 
   if(!res){
      return NULL;
   }
   memmove(res,"...",3); 
   memmove(res+3,path+(strlen(path)-(PROMPT_CHAR_MAX-3)),PROMPT_CHAR_MAX-2);
   return res;
} //need to free char* returned by fonc + if path == NULL return NULL
//trunc path from left

char* create_prompt (char *path, int job_number){

bool istrunc = false;
size_t s_path = strlen(path);
if(s_path > 25){

   path = truncate_to_size(path);

   if(path == NULL){
      goto error;
   }
   s_path=25;
   istrunc = true;

}
size_t length = 26+s_path+1; //26= size of needed char for everything other than path pas assez alloc ici

char *res = malloc(length); 
if(!res){
   goto error;
}
memmove(res,"\001\033[33m\002[",8);
sprintf(res+8,"%d",job_number);
char *res2 = "]\001\033[35m\002"; 
memmove(res+9,res2,8);
memmove(res+17,path,s_path);
char *res3 = "\001\033[00m\002$ ";
memmove(res+17+s_path, res3,9);
memmove(res+17+s_path+9,"\0",1);

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
   return 4;


}

