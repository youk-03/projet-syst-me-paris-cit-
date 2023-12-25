#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "job.h"
#include "jobs_command.h"


void print_jobs(job * job, int fd){ //fd is 1 or 2

    dprintf(fd,"[%d] %d ",job->id, job->job_pid);
    switch (job->status)
    {
    case 1:
        dprintf(fd,"Running");
        break;
    case 2:
        dprintf(fd,"Stopped");
        break;
    case 3:
        dprintf(fd,"Detached");
        break;
    case 4:
        dprintf(fd,"Killed");
        break;
    case 5:
        dprintf(fd,"Done");
        break;

    case -2:
     dprintf(fd,"Stopped"); //(explained in processus.c line 129)
     break;    

    default:
        goto error;
        break;
    }
    dprintf(fd," %s\n",job->name);
    return;

    error: 
    perror("Print jobs");
    exit(1);
}

void print_table_of_jobs(job_table * job_table){
for (int i=0; i<job_table->length; i++){
    print_jobs(job_table->table[i],1);
}
}

void print_table_of_certain_jobs(job_table * job_table, int number){
    for (int i=0; i<job_table->length; i++){
        if(job_table->table[i]->id==number){
            print_jobs(job_table->table[i],1);
        }

    }
    
}


int jobs(bool option, char * arg, job_table * job_table){ // option = -t is present ; arg = job number (%2)

char * string_jobnumber=NULL;

if(job_table == NULL){
    return 1;
}   
 
if(option) {

    printf("Not implemented yet ):");

    return 1;
}

if (arg==NULL){
    print_table_of_jobs(job_table);//
    return 0;

} else if (arg[0]=='%'){

    string_jobnumber=malloc(strlen(arg));  
    if(string_jobnumber==NULL) {
        perror("malloc");
        exit(1);
    }
    memcpy(string_jobnumber,arg+1,strlen(arg));
   
   print_table_of_certain_jobs(job_table,atoi(string_jobnumber));
 
    free(string_jobnumber);
    return 0;

} else {
    perror("jobs");
   return 1 ;
}
  fflush(NULL);

return 0;

}

void maj_main_print(job_table* job_table, bool stdout){
    int ic = 0;
    int fd=2;
    if(stdout){
        fd=1;
    }

    while(ic < job_table->length){
        switch(job_table->table[ic]->status){
            case 1: /*nothing to do*/ ic++; break; //running
            case 2: print_jobs(job_table->table[ic],fd); ic++; break; //stopped
            case 3: /*nothing to do*/ic++;  break; //detached
            case 4: print_jobs(job_table->table[ic],fd); delete_job(job_table->table[ic], job_table); break; //killed
            case 5: print_jobs(job_table->table[ic],fd); delete_job(job_table->table[ic], job_table); break; //done
            default: ic++; break;
        }
    }

}

//why using a ic ?
//bc deletejob do a memmove to shift all the job so if i delete 2
// 3 become the next 2 but in a for loop i goes to 3 without looking at the new 2
