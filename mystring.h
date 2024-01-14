#ifndef MYSTRING_H
#define MYSTRING_H
#include <stddef.h>


struct argument {
  char** data; //line returned by split
  int nbr_arg; //number of words in data (size of the array)  //NULL is not counted in nbr_arg
};
typedef struct argument argument;

argument* split(const char* tosplit, char delim );
//return NULL if tosplit == NULL

 void free_argument(argument* tofree);

argument* cpy_argument (argument* arg, int end);

argument* cpy_argument_end (argument* arg, int start);
  
#endif // MYSTRING_H
