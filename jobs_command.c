#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "job.h"
#include "jobs_command.h"

#define MAX_ALLOC 1000


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


int jobs(bool option, char * arg, job_table * job_table){ // option = -t is present ; arg = job number (%2)

char * string_jobnumber=NULL;

if(job_table == NULL){
    return 1;
}   
 
if (arg==NULL){ 

    if(option==true) {

    for (int i=0; i<job_table->length; i++){
        print_jobs(job_table->table[i],1);
        if (job_table->table[i]->status!=4 && job_table->table[i]->status!=5) // if job not done/killed
            jobs_t(job_table->table[i]->job_pid,1);
    }

    return 0;
        } else {
    print_table_of_jobs(job_table);
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

        for (int i=0; i<job_table->length; i++){
            
        if (atoi(string_jobnumber)==job_table->table[i]->id){
            print_jobs(job_table->table[i],1);
            if (job_table->table[i]->status!=4 && job_table->table[i]->status!=5) // if job not done/killed
                jobs_t(job_table->table[i]->job_pid,1);
        } 
    }
    free(string_jobnumber);
    return 0;
    } else {
    
   
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
            case 3: print_jobs(job_table->table[ic],fd); delete_job(job_table->table[ic], job_table) ;ic++;  break; //detached
            case 4: print_jobs(job_table->table[ic],fd); delete_job(job_table->table[ic], job_table); break; //killed
            case 5: print_jobs(job_table->table[ic],fd); delete_job(job_table->table[ic], job_table); break; //done
            default: ic++; break;
        }
    }

}

//why using a ic ?
//bc deletejob do a memmove to shift all the job so if i delete 2
// 3 become the next 2 but in a for loop i goes to 3 without looking at the new 2



//tests if job is detached 

// int is_detached(job_table * job_table, int job_id) {

//     int bash_pid=getppid(); // jsh's pid
//     char proc_stat_filename[MAX_ALLOC];
//     char other_char[MAX_ALLOC];
//     char state;
//     int other, ppid; 
//     int scan=-1 ;
//     int nb_unfinished=0;

//     for (int i=0 ; i<job_table->length ; i++) {

//         if (job_id==job_table->table[i]->id){ //check matching job id

//                 if (job_table->table[i]->status!=4 && job_table->table[i]->status!=5) { //job status can't be killed or done, proc file doesn't exist
                
//                 sprintf(proc_stat_filename, "/proc/%d/stat",job_table->table[i]->id);

//                 FILE *f_stat = fopen(proc_stat_filename, "r");
//                 if (f_stat==NULL) {
//                     perror("jobs : open");
//                     exit(1);
//                 }

            
//                 scan=fscanf(f_stat, "%d %s %c %d",&other, other_char, &state, &ppid);

//                 if(scan==-1) {
//                     perror("jobs: scanf");
//                     exit(1);
//                 }

//                 fclose(f_stat);
            
//                 if (ppid==bash_pid){  // if launched by bash and unfinished, then job is not detached. else nb of unfinished jobs not launched by bash +1 
//                     return 1 ;
//                 } else {
//                     nb_unfinished++;
//                 }

            
//             }

//         }

//     } 
    
//     if (nb_unfinished>0) return 0;
//     else return 1;

// }
