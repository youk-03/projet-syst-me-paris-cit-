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


void jobs(bool argument,processus_table * processus_table){
if(argument) {
    printf("Not implemented yet ):");
}
else {
    printTableOfJobs(processus_table);
}

}
