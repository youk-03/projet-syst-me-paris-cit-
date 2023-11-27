
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mystring.h"
#include <stdbool.h>
#include <stddef.h>

struct string * string_new(size_t capacity) {
  if (capacity == 0) return NULL;
  char* data = malloc(capacity);
  if (!data) { return NULL; }
  struct string * str = malloc(sizeof(*str));
  if (!str) {
    free(data);
    return NULL;
  }
  str->capacity = capacity;
  str->length = 0;
  str->data = data;
  str->data[0] = 0;
  return str;
}

void string_delete(struct string * str) {
  free(str->data);
  free(str);
}

int string_append (struct string * dest, char * src) {

size_t size_s2 = strlen(src) + 1; // pour 0
size_t final_length = dest->length + size_s2;
if(dest->capacity < final_length){
  printf("dest capacity too low");
  // realloc *2 a la place ///////////////////////////////////////////////////////::
  return 0;
}

memmove(dest->data+dest->length, src, size_s2);

dest->length+= size_s2 - 1;

  return 1;
}

void string_truncate (struct string * str, size_t nchars) {
  if (nchars > str->length) { nchars = str->length; }
  str->data[str->length-nchars] = 0;
  str->length -= nchars;
}

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

  size_t tosplit_l = strlen(tosplit);
   //variable

  if(tosplit == NULL){
    return NULL;
  }
   //Case where tosplit == NULL


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

  if(nbr_words == 0 && strcmp(tosplit," ")!=0){
    nbr_words=1; 
  }
  //case where there is no delim and tosplit is not only " "

  res= malloc(nbr_words*sizeof(char*));

  if(!res){
    goto error; //GERER LE CAS ICI
  }

  is_delim=true;
  for(size_t i = 0; i<tosplit_l; i++){
    if(tosplit[i] == delim){
      if(!is_delim){
      end=i-1;
      is_delim=true;
      if(end >= start){
        res[words_it] = calloc((end-start+1)*sizeof(char)+1,(end-start+1)*sizeof(char)+1); //\0 + we count starting by 0
        //malloc ici avant steven
      if(!res[words_it]){
        goto error; //GERER LE CAS ICI AUSSI
      }
        strncpy(res[words_it],tosplit+start,end-start+1); //same
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
    res[words_it] = calloc((end-start+1)*sizeof(char)+1,(end-start+1)*sizeof(char)+1); 
    strncpy(res[words_it],tosplit+start,end-start+1); 
  }
  //if tosplit don't finish with delim last word is not in res, doing so here

  data = malloc(sizeof(argument));
  data->data=res;
  data->nbr_arg=nbr_words;
  //initializing the struct argument
  return data;

  error:
  //free ce qu'il y a a free et lancé fonc command exit ///////////////////////////////////////////////////////////:
  exit(0);

}

// int main (int argc, char **argv){
//   char* test= " c  o u c   o  u c ' e s  t mo i      encor e       ";

 
//   argument* tab = split(test,' ');
//   int nbr_words = tab->nbr_arg; 
//   printf("number : %d\n",nbr_words);

//   for(int i=0; i<nbr_words; i++){
//     printf("%s\n",tab->data[i]);
// }

// free_argument(tab);

// return 1;

  


// }

