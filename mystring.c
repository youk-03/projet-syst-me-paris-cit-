
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mystring.h"
#include <stdbool.h>
#include <stddef.h>
#include "interrogation_exit.h"
#include <readline/readline.h>
#include <readline/history.h>


void free_argument(argument* tofree){
  for(int i=0; i<tofree->nbr_arg; i++){
    free(tofree->data[i]);
    tofree->data[i]=NULL;

  }
  free(tofree->data);
  tofree->data=NULL;
  free(tofree);
  tofree=NULL;
}

//Return NULL if any arg is NULL
argument* split(const char* tosplit, char delim ){

  argument* data = NULL;

  int nbr_words=0;
  bool is_delim=true;

  int start=0;
  int end=0;
  int words_it = 0;
  char** res= NULL;


  if(tosplit == NULL){
    return NULL;
  }
   //Case where tosplit == NULL

   size_t tosplit_l = strlen(tosplit);

   if(strcmp(tosplit,"")==0){
    return NULL;
   }


  for (size_t i=0; i<tosplit_l; i++){
    if(tosplit[i] == delim){
      is_delim=true;
    }
    else{
      if(is_delim){
        is_delim=false;
        nbr_words++;
      }
    }
  }
  //initializing nbr_words
  if(nbr_words == 0 && tosplit[0] == ' '){
    return NULL;
  }

  if(nbr_words == 0 && strcmp(tosplit," ")!=0){
    nbr_words=1; 
  }
  //case where there is no delim and tosplit is not only " "


  res= malloc((nbr_words+1)*sizeof(char*)); //+1 to add null at the very end

  if(!res){
    goto error; 
  }

  is_delim=true;
  for(size_t i = 0; i<tosplit_l; i++){
    if(tosplit[i] == delim){
      if(!is_delim){
      end=i-1;
      is_delim=true;
      if(end >= start){
        res[words_it] = malloc((end-start+1)*sizeof(char)+1); //\0 + we count starting by 0

      if(!res[words_it]){
        goto error; 
      }
        strncpy(res[words_it],tosplit+start,end-start+1); //same
        res[words_it][end-start+1]='\0';
        words_it++;
      }
      }
    }
    else{
      if(is_delim){
        is_delim=false;
        start=i;
      }
    }

  } //initializing res**

  if(!is_delim){
    end = tosplit_l-1;
    res[words_it] = malloc((end-start+1)*sizeof(char)+1);
    if(!res[words_it]){
      goto error; 
    }
    strncpy(res[words_it],tosplit+start,end-start+1); 
    res[words_it][end-start+1]='\0';
  }
  //if tosplit don't finish with delim last word is not in res, doing so here
  res[nbr_words] = NULL; //for exec

  data = malloc(sizeof(argument));
  if(!data){
    goto error; 
  }
  data->data=res;
  data->nbr_arg=nbr_words;
  //initializing the struct argument
  return data;

  error:
  perror("in split: ");
  exit(1); 

}

argument* cpy_argument (argument* arg, int end){

  argument* res = malloc(sizeof(argument));
  if(!res) goto error;
  //0 1 2 3 4 5 6 
  res->nbr_arg = end;

  res->data = malloc(sizeof(char*)*(res->nbr_arg+1));//exec
  if(!res->data) goto error;
  for(int i=0;i<res->nbr_arg; i++){

    res->data[i] = malloc(strlen(arg->data[i])+1);
    res->data[i] = strcpy(res->data[i], arg->data[i]);
    if(!res->data[i]){
      goto error;
    }

  }

  res->data[res->nbr_arg] = NULL;

  return res;

  error:
  perror("cpy_arg");
  exit(1);
}

argument* cpy_argument_end (argument* arg, int start){

  argument* res = malloc(sizeof(argument));
  if(!res) goto error;

  res->nbr_arg = arg->nbr_arg - start;

  res->data = malloc(sizeof(char*)*(res->nbr_arg+1));//exec
  if(!res->data) goto error;
  for(int i=0;i<res->nbr_arg; i++){

    res->data[i] = malloc(strlen(arg->data[i+start])+1);
    res->data[i] = strcpy(res->data[i], arg->data[i+start]);
    if(!res->data[i]){
      goto error;
    }

  }

  res->data[res->nbr_arg] = NULL;

  return res;

  error:
  perror("cpy_arg");
  exit(1);
}

