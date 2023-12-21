#include "processus.h"
#include "jobs.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
//processus_table

static int id = 1;


processus_table* allocate_processus_table(size_t capacity){
    processus_table* res = malloc(sizeof(processus_table));
    if(!res) goto error;

    res->capacity = capacity;
    res->length = 0;
    res->table = malloc(sizeof(processus*)*capacity);

    if(!res->table) goto error;

    return res;
    error:
    //??? exit_jsh ?????????????????????????????????????????????????????????????,
}

void free_processus_table(processus_table* table){

    for(int i=0; i<table->length; i++){
       free_processus(table->table[i]);
    }

    free(table->table);
    table->table = NULL;
    free(table);
    table=NULL;

}

void add_processus (processus* processus, processus_table* table ){

    if(table->capacity == table->length){
        table->capacity = table->capacity*2;
        table->table = realloc(table->table, sizeof(processus)*table->capacity);
        if(!table->table){
            goto error;
        
        }
    }

    table->table[table->length] = processus; 
    table->length++;

    return;

    error:
    exit(1);

}

int delete_processus (processus* processus, processus_table* table){

    int i = 0;
    bool deleted = false;

    for(i; i<table->length; i++){
        if(table->table[i] == processus){
            free_processus(table->table[i]);
            deleted = true;
            break;
        }

    }

    if(deleted){
        memmove(table->table+i,table->table+i+1,(table->length-i-1)*sizeof(processus));
        table->length--;
          if(table->length == 0){
        id = 1;
    }
    }
    else{
        return -1;
    }

    return 0;

}

//processus

void free_processus (processus* proc){

    free(proc->name); 
    free(proc);
    proc=NULL;
}

processus* allocate_processus (pid_t process_pid, pid_t father_pid, int status, char* name){
    processus* res = malloc(sizeof(processus));
    
    if(!res) goto error;

    res->process_pid = process_pid;
    res->father_pid = father_pid;
    res->status = status;
    res->name = malloc(strlen(name)+1); 
    res->name = strcpy(res->name,name);
    if(!res->name) goto error;
    res->id = id;
    id++;


    return res;

    error:
    //exit_jsh
}


void print_test (processus_table* table){
    for(int i=0; i<table->length; i++){

        printf("je suis: %d, je m'appelle: %s\n", table->table[i]->process_pid, table->table[i]->name);

    }
}

int maj_process_table (processus_table* proc_table){
    int status = -1;
    int wait = -1;

    for(int i=0; i<proc_table->length; i++){
       wait =  waitpid(proc_table->table[i]->process_pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
       if(wait == 0){
          if(proc_table->table[i]->status == 2){
           proc_table->table[i]->status =-2; //so that it won't print each time "... STOPPED" 
        }
        continue;
       }
       if(wait == -1){
         if(errno == ECHILD){
            proc_table->table[i]->status = 5; // 4 ?
        
        }
    else goto error;
       }

       else{
        

        if(WIFEXITED(status)){ //end with exit or return
            proc_table->table[i]->status = 5; //done
        }
        else if(WIFSIGNALED(status)){ //terminated with signal
            proc_table->table[i]->status = 4; //killed
        }
        else if(WIFSTOPPED(status)){
            proc_table->table[i]->status = 2; //stopped
        }
        else if(WIFCONTINUED(status)){
            proc_table->table[i]->status = 1; //continue
            print_jobs(proc_table->table[i],2);
        }

       }
    }
    maj_main_print(proc_table);
    return 0;

    error:
   
    perror("maj_process_table");
    return 1;
}

// int main (int argc, char** argv){

//     processus *test1 = NULL;
//     processus *test2;
//     processus *test3;
//     processus_table* table_test;
//     char* test1_name = "coucou";
//     char* test2_name = "sleep";
//     char* test3_name = "wait";
//     test1 = allocate_processus(11111, 0, 2, test1_name, 0);
//     test2 = allocate_processus(22222, 0, 2, test2_name, 1);
//     test3 = allocate_processus(33333, 0, 2, test3_name, 2);
//     table_test = allocate_processus_table(2);
//     add_processus(test1,table_test);
//     add_processus(test2,table_test);
//     add_processus(test3,table_test);

//     print_test(table_test);

//     puts(" ");

//     delete_processus(test1,table_test);
//     //delete_processus(test2,table_test);
//     delete_processus(test3,table_test);

//      print_test(table_test);

    

//     free_processus_table(table_test);

//     //printf("test1: pid: %d, ppid: %d, status: %d, nom: %s, id: %d", test1->process_pid, test1->father_pid, test1->status, test1->name, test1->id);

//     //free_processus(test1);
//     //free_processus(test2);
//     //free_processus(tesàt3);

//     return 1;
// }