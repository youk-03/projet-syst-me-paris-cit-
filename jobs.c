#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "processus.h"
#include "jobs.h"


void print_jobs(processus * processus){

    dprintf(2,"[%d] %d ",processus->id, processus->process_pid);
    switch (processus->status)
    {
    case 1:
        dprintf(2,"Running");
        break;
    case 2:
        dprintf(2,"Stopped");
        break;
    case 3:
        dprintf(2,"Detached");
        break;
    case 4:
        dprintf(2,"Killed");
        break;
    case 5:
        dprintf(2,"Done");
        break;

    case -2:
     dprintf(2,"Stopped"); //(explained in processus.c line 129)
     break;    

    default:
        goto error;
        break;
    }
    dprintf(2," %s\n",processus->name);
    return;

    error: 
    perror("Print jobs");
    exit(1);
}

void print_table_of_jobs(processus_table * proc_table){
for (int i=0; i<proc_table->length; i++){
    print_jobs(proc_table->table[i]);
}
}

void print_table_of_certain_jobs(processus_table * proc_table, int number){
    for (int i=0; i<proc_table->length; i++){
        if(proc_table->table[i]->id==number){
            print_jobs(proc_table->table[i]);
        }

    }
    
}


int jobs(bool option, char * arg, processus_table * processus_table){ // option = -t is present ; arg = job number (%2)

char * string_jobnumber=NULL;

if(processus_table == NULL){
    return 1;
}   
 
if(option) {

    printf("Not implemented yet ):");

    return 1;
}

if (arg==NULL){
    print_table_of_jobs(processus_table);
    return 0;

} else if (arg[0]=='%'){

    string_jobnumber=calloc(strlen(arg),'0'); 
    if(string_jobnumber==NULL) {
        perror("malloc");
        exit(1);
    }
    memcpy(string_jobnumber,arg+1,strlen(arg)-1);
   
   print_table_of_certain_jobs(processus_table,atoi(string_jobnumber));
 
    free(string_jobnumber);
    return 0;

} else {
    perror("jobs");
   return 1 ;
}
  fflush(NULL);

return 0;

}

void maj_main_print(processus_table* proc_table){
    int ic = 0;

    while(ic < proc_table->length){
        switch(proc_table->table[ic]->status){
            case 1: /*nothing to do*/ ic++; break; //running
            case 2: print_jobs(proc_table->table[ic]); ic++; break; //stopped
            case 3: /*nothing to do*/ic++;  break; //detached
            case 4: print_jobs(proc_table->table[ic]); delete_processus(proc_table->table[ic], proc_table); break; //killed
            case 5: print_jobs(proc_table->table[ic]); delete_processus(proc_table->table[ic], proc_table); break; //done
            default: ic++; break;
        }
    }

}

//why using a ic ?
//bc delete processus do a memmove to shift all the processus so if i delete 2
// 3 become the next 2 but in a for loop i goes to 3 without looking at the new 2
