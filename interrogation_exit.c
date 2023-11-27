#include <stdio.h>
#include <stdlib.h>
#include "interrogation_exit.h"


void interrogation_point (int val){
    printf("\nReturn value of last command : %d\n", val);
}

void exit_jsh (int val){
    exit(val);
}