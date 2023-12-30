#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "processus.h"
#include "jobs.h"

#define MAX_ALLOC 1000


void print_jobs(processus * processus, int fd){ //fd is 1 or 2

    dprintf(fd,"[%d] %d ",processus->id, processus->process_pid);
    switch (processus->status)
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
    dprintf(fd," %s\n",processus->name);
    return;

    error: 
    perror("Print jobs");
    exit(1);
}

void print_table_of_jobs(processus_table * proc_table){
for (int i=0; i<proc_table->length; i++){
    print_jobs(proc_table->table[i],1);
}
}

void print_table_of_certain_jobs(processus_table * proc_table, int number){
    for (int i=0; i<proc_table->length; i++){
        if(proc_table->table[i]->id==number){
            print_jobs(proc_table->table[i],1);
        }

    }
    
}


int option_t(int pid, int fd){
    

    char proc_stat_filename[MAX_ALLOC];
    char proc_comm_filename[MAX_ALLOC];
    sprintf(proc_stat_filename, "/proc/%d/stat", pid);
    sprintf(proc_comm_filename, "/proc/%d/comm", pid);

    FILE *f_stat = fopen(proc_stat_filename, "r");
    if (f_stat==NULL) {
        perror("jobs : open");
        exit(1);
    }
    FILE *f_test = fopen(proc_comm_filename, "r");
    if (f_test==NULL) {
        perror("jobs : open");
        exit(1);
    }

    int other;
    char other_char[MAX_ALLOC];
    char comm[MAX_ALLOC];
    char state;
    int ppid;
    int first_scan=-1 ;
    int second_scan=-1 ;
    first_scan=fscanf(f_stat, "%d %s %c %d",&other, other_char, &state, &ppid);

    if(first_scan==-1) {
        perror("jobs: scanf");
        exit(1);
    }
    second_scan=fscanf(f_test, "%d %s",&other, comm);

    if(second_scan==-1) {
        perror("jobs: scanf");
        exit(1);
    }

    if(fgets(comm, 500, f_test)==NULL) {
        perror("jobs: fgets");
        exit(1);
    }

    printf("    | %d ", pid);
    fflush(NULL);

    // TODO: clear this up

    switch (state) {  
    case 'R':
        dprintf(fd,"Running");
        break;
    case 'T':
        dprintf(fd,"Stopped");
        break;
    case 'Z':
        dprintf(fd,"Done"); // zombie = done ????
        break;
    case 'S':  // for S and D: sleeping == stopped????
        dprintf(fd,"Sleeping");
        break;
    case 'D':
        dprintf(fd,"Sleeping");
        break;    

    default:
        printf("ERROR");
        goto error ;
        break;
    }
    printf(" %s\n", comm);
    fflush(NULL);
    fclose(f_stat);
    fclose(f_test);

    return 0;

    error : 
    if (f_stat!=NULL) fclose(f_stat);
    if (f_test!=NULL) fclose(f_test);
    exit(1);

}

int jobs_t(int pid, int fd){
    char proc_children_filename[MAX_ALLOC];
    char * children_pid=malloc(MAX_ALLOC);

    if(children_pid==NULL){
        perror("jobs: malloc");
        exit(1);
    }

    char *children;
    sprintf(proc_children_filename, "/proc/%d/task/%d/children",pid,pid);
    FILE *f_children=fopen(proc_children_filename,"r");

    if(f_children==NULL) {
        perror("jobs: fopen");
        goto error; 
    }

    while (fgets(children_pid,100,f_children)!=NULL){
        
        children = strtok(children_pid, " ");
        int init_child=atoi(children);
        option_t(init_child,fd);
        while(children) {
                
            children=strtok(NULL," ");
            if (children!=NULL){
            
            int children_int=atoi(children);
            option_t(children_int,fd);
            jobs_t(children_int,fd);
            }
        }

    }
    free(children_pid);
    fclose(f_children);
    return 0;

    error :
    if (children_pid!=NULL) free (children_pid);
    if (f_children!=NULL) fclose(f_children);
    return 1;
}






int jobs(bool option, char * arg, processus_table * processus_table){ // option = -t is present ; arg = job number (%2)

char * string_jobnumber=NULL;

if(processus_table == NULL){
    return 1;
}   

if (arg==NULL){ 

    if(option==true) {

    for (int i=0; i<processus_table->length; i++){
        print_jobs(processus_table->table[i],1);
        if (processus_table->table[i]->status!=4 && processus_table->table[i]->status!=5) // if process not done/killed
            jobs_t(processus_table->table[i]->process_pid,1);
    }

    return 0;
        } else {
    print_table_of_jobs(processus_table);
    return 0;
    }

} else if (arg[0]=='%'){

    string_jobnumber=calloc(strlen(arg),'0'); 
    if(string_jobnumber==NULL) {
        perror("malloc");
        exit(1);
    }
    memcpy(string_jobnumber,arg+1,strlen(arg)-1);

    if (option==true){ 

        for (int i=0; i<processus_table->length; i++){
            
        if (atoi(string_jobnumber)==processus_table->table[i]->id){
            print_jobs(processus_table->table[i],1);
            if (processus_table->table[i]->status!=4 && processus_table->table[i]->status!=5) // if process not done/killed
                jobs_t(processus_table->table[i]->process_pid,1);
        } 
    }
    free(string_jobnumber);
    return 0;
    } else {
    
   
   print_table_of_certain_jobs(processus_table,atoi(string_jobnumber));
 
    free(string_jobnumber);
    return 0;
    }

} else {
    perror("jobs");
   return 1 ;
}
  fflush(NULL);

return 0;

}

void maj_main_print(processus_table* proc_table, bool stdout){
    int ic = 0;
    int fd=2;
    if(stdout){
        fd=1;
    }

    while(ic < proc_table->length){
        switch(proc_table->table[ic]->status){
            case 1: /*nothing to do*/ ic++; break; //running
            case 2: print_jobs(proc_table->table[ic],fd); ic++; break; //stopped
            case 3: /*nothing to do*/ic++;  break; //detached
            case 4: print_jobs(proc_table->table[ic],fd); delete_processus(proc_table->table[ic], proc_table); break; //killed
            case 5: print_jobs(proc_table->table[ic],fd); delete_processus(proc_table->table[ic], proc_table); break; //done
            default: ic++; break;
        }
    }

}

//why using a ic ?
//bc delete processus do a memmove to shift all the processus so if i delete 2
// 3 become the next 2 but in a for loop i goes to 3 without looking at the new 2
