#include <stdio.h>
#include <errno.h>
#include <string.h>
#include<unistd.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "mystring.h"
#define PROMPT_CHAR_MAX 25

char* truncate_to_size (char* path){
   if(path == NULL){
      return NULL;
   }
   char *res = malloc(PROMPT_CHAR_MAX+1); 
   memmove(res,"...",3); 
   memmove(res+3,path+(strlen(path)-(PROMPT_CHAR_MAX-3)),PROMPT_CHAR_MAX-2);
   return res;
} //need to free char* returned by fonc + if path == NULL return NULL
//trunc path left

char* create_prompt (char *path, int job_number){

size_t s_path = strlen(path);
if(s_path > 25){

   path = truncate_to_size(path);
   s_path=25;

}
size_t length = 25+s_path+1; //25= size of needed char for everything other than path

char *res = malloc(length); 
memmove(res,"\001\033[33m\002[",8);
sprintf(res+8,"%d",job_number);
char *res2 = "]\001\033[35m\002"; 
memmove(res+9,res2,8);
memmove(res+17,path,s_path);
char *res3 = "\001\033[00m\002$ ";
memmove(res+17+s_path, res3,9);
memmove(res+17+s_path+9,"\0",1);
free(path);
return res;

} //need to free res*

int main (int argc, char* argv[]){

   char* s;
   char *pwd = getcwd(NULL,0);
   char *prompt = create_prompt(pwd,0);
   argument* arg =NULL;
   while(1){
   s = readline(prompt); 
   arg= split(s,' ');
   for(int i=0;i<arg->nbr_arg;i++){
      printf("%s\n",arg->data[i]);
   }
   free_argument(arg);
   arg=NULL;
   free(s);
   s=NULL;
   //sleep(1);
   }


   //TEST

   // argument *a;
   // char* copy= "12 3 4 5 ";

   // while(1){
      
   //    a=split(copy,' ');
   //    for(int i=0; i<a->nbr_arg; i++){
   //       printf("%s\n",a->data[i]);
   //    }
   //     for(int i=0; i<a->nbr_arg; i++){
   //       free(a->data[i]);
   //       a->data[i]=NULL;
   //    }
   //    free(a->data);
   //    a->data=NULL;
   //    free(a);
   //    a=NULL;
   //    sleep(1);
   // }

   return 0;
}