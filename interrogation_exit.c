#include <stdio.h>
#include <stdlib.h>
#include "interrogation_exit.h"


int interrogation_point (int val){
    printf("\nReturn value of last command : %d\n", val);
    return 0;
}

void exit_jsh (int val){
    exit(val);
}