#include <stdio.h>
#include <errno.h>
#include <string.h>

void interrogation_point (int val){
    printf("\nReturn value of last command : %d\n", val);
}

void exit_ (int val){
    exit_(val);
}

int main (int argc, char *argv[]){
    static int last_return = 0;

    interrogation_point(last_return);
    exit_(last_return);

    return 0;
}
