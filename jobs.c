#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#include "processus.h"
#include "jobs.h"


void printJobs(processus * processus){

    printf("[%d] %d ",processus->id, processus->process_pid);
    switch (processus->status)
    {
    case 1:
        printf("Running");
        break;
    case 2:
        printf("Stopped");
        break;
    case 3:
        printf("Detached");
        break;
    case 4:
        printf("Killed");
        break;
    case 5:
        printf("Done");
        break;

    case -2:
     printf("Stopped"); //(explained in processus.c line 129)
     break;    

    default:
        goto error;
        break;
    }
    printf(" %s\n",processus->name);
    return;

    error: 
    perror("Print jobs");
    exit(1);
}

void printTableOfJobs(processus_table * proc_table){
for (int i=0; i<proc_table->length; i++){
    printJobs(proc_table->table[i]);
}
}


int jobs(bool argument,processus_table * processus_table){

 if(processus_table == NULL){
    return 1;
 }   
 
if(argument) {
    printf("Not implemented yet ):");

    return 1;
}
else {
    printTableOfJobs(processus_table);
}

return 0;

}

void maj_main_print(processus_table* proc_table){
    int ic = 0;

    while(ic < proc_table->length){
        switch(proc_table->table[ic]->status){
            case 1: /*nothing to do*/ ic++; break; //running
            case 2: printJobs(proc_table->table[ic]); ic++; break; //stopped
            case 3: /*nothing to do*/ic++;  break; //detached
            case 4: printJobs(proc_table->table[ic]); delete_processus(proc_table->table[ic], proc_table); break; //killed
            case 5: printJobs(proc_table->table[ic]); delete_processus(proc_table->table[ic], proc_table); break; //done
            default: ic++; break;
        }
    }

}

//why using a ic ?
//bc delete processus do a memmove to shift all the processus so if i delete 2
// 3 become the next 2 but in a for loop i goes to 3 without looking at the new 2
