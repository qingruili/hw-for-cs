#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

#define MAX_LEN 40

// state enum for process
typedef enum State_a State;
enum State_a{
    REDAY,
    RUNNING,
    BLOCK_RECV,
    BLOCK_SEND,
    BLOCK_SEM,
};

// PCB structure
typedef struct PCB_a PCB;
struct PCB_a
{
    int pid;
    int priority;
    State state;
    char storeMessage[MAX_LEN+1];
    List* sender_list;
};

//Semaphore structure
typedef struct Semaphore_a Semaphore;
struct Semaphore_a
{
    int sid;
    List* process;
    int val;
};


List* ready_list; // process be ready to run
List* block_list; // process be blocked
Semaphore* sem[5];
int sem_num = 0; // count number of semaphore
int pid_num = 0; // count number of process
bool running = true;


PCB* initial;

// help function
PCB* PCB_create(int priority);
void addtoQ(PCB* pcb);
PCB* search(List* list, int pid);
bool compare(void* it, void* p);
void remove_sem(PCB* pcb);
void remove_queue(List* list, PCB* pcb);
void free_pcb(PCB* pcb);
PCB* remove_psb(List* list, int pid);
Semaphore* Semaphore_create(int semaphore, int val);
void free_sem(Semaphore* se);


//function for simulation
void Create(int pri);
void Fork();
void Kill(int pid);
void Exit();
void Quantum();
void Send(int pid, char* m);
void Receive();
void Replay(int pid, char* word);
void New_Semaphore(int semaphore, int num);
void SempahoreP(int sth);
void SempahoreV(int semaphore);
void Procinfo(int pid);
void Totalinfo();


int main(){

   
    char tag;
    int sth;
    char* m = malloc(40 * sizeof(char));
    int v;
    // initialize the list and semaphore
    block_list = List_create();
    ready_list = List_create();
    for(int i = 0; i < 5; i++){
        sem[i] = NULL;
    }

    initial = PCB_create(3);
    List_append(ready_list, initial);


    while(running){
         printf("\n*********************************************\n");
         printf("****************System Status****************\n");

        // show running process
        PCB* pcb = List_first(ready_list);
        printf("Current Process: pid=%d", pcb->pid);
        if (pcb->pid == 0) {
            printf (" (Init)");
        }
        printf(", priority=%d\n", pcb->priority);
        // show ready number
        printf("Ready processes number: %d\n", List_count(ready_list) - 1);
        // show blocked number
        printf("Blocked processes number: %d\n", List_count(block_list));
        // show semaphore number
        printf("Semaphores number: %d\n", sem_num);
        printf("*********************************************\n");
        printf("*********************************************\n\n");

        char* word = malloc(sizeof(char));// type alphabets
        scanf("%s",word);
        if (strlen(word) > 1)
            tag = 'A';
        else
            tag = word[0];

        switch(tag){
            case 'C':
                printf("Enter your priority (0-2): ");
                scanf("%d", &sth);
                while(sth > 2 || sth < 0){
                    printf("priority is not recognized. Please try again.\n(Process ID can only be from 0-2): ");
                    scanf("%d", &sth);
                }
                Create(sth);
                break;
            
            case 'F':
                Fork();
                break;
            
            case 'K':
                printf("Enter the process ID you want to kill:");
                scanf("%d", &sth);
                Kill(sth);
                break;
            
            case 'E':
                Exit();
                break;
            
            case 'Q':
                Quantum();
                break;
            
            case 'S':
                printf("Enter the process ID you want to send message: ");
                scanf("%d", &sth);
                printf("Enter the message:");
                scanf("%s",m);
                while( strlen(m) > 40){
                    printf("No message was found. Please try again.\nEnter the message: ");
                    scanf("%s", m);
                }
                Send(sth, m);
                break;
            case 'R':
                Receive();
                break;

            case 'Y':
                printf("Enter the process ID you want to send message: ");
                scanf("%d", &sth);
                printf("Enter the message:");
                scanf("%s",m);
                while( strlen(m) > 40){
                    printf("No message was found. Please try again.\nEnter the message: ");
                    scanf("%s", m);
                }
                Replay(sth, m);
                break;
            case 'N':
                printf("Enter the semaphore ID you want to create: ");
                scanf("%d", &sth);
                while(sth >= 5 || sth < 0){
                    printf("Process ID not recognized. Please try again.\n(Process ID can only be from 0-2)\nEnter the process ID you would like to send message: ");
                    scanf("%d", &sth);
                }
                printf("Enter the semphore value:");
                scanf("%d",&v);
                while(v < 0){
                    printf("Semaphore number must be positive. Please try again.\nEnter the message: ");
                    scanf("%d",&v);
                }
                New_Semaphore(sth, v);
                break;
            
            case 'P':
                printf("Enter the semaphore ID you want to use: ");
                scanf("%d", &sth);
                while(sth >= 5 || sth < 0){
                    printf("Process ID not recognized. Please try again.\n(Process ID can only be from 0-2)\nEnter the process ID you would like to send message: ");
                    scanf("%d", &sth);
                }
                SempahoreP(sth);
                break;
            
            case 'V':
                printf("Enter the semaphore ID you want to send message: ");
                scanf("%d", &sth);
                while(sth >= 5 || sth < 0){
                    printf("Process ID not recognized. Please try again.\n(Process ID can only be from 0-2)\nEnter the process ID you would like to send message: ");
                    scanf("%d", &sth);
                }
                SempahoreV(sth);
                break;
            
            case 'I':
                printf("Enter the process ID you want to show: ");
                scanf("%d", &sth);
                Procinfo(sth);
                break;
            
            case 'T':
                Totalinfo();
                break;

            default:
                printf("This command is invalid.You should enter alphabets from following: \n");
                printf("[C] [F] [K] [E] [Q] [S] [R] [Y] [N] [P] [V] [I] [T]\n");
                printf("Please try enter again.\n");
                break;
        }
    }
    //free the list and semaphore
    printf("\n Simulation Done\n");
    List_free(ready_list, NULL);
    List_free(block_list, NULL);
    for( int i = 0; i < 5; i++){
        free_sem(sem[i]);
    }
    return 0;
}







// intialize PCB struct
PCB* PCB_create(int priority){
    PCB* pcb = malloc(sizeof(PCB));
    pcb->pid = pid_num++;
    pcb->priority = priority;
    pcb->state = REDAY;
    pcb->sender_list = List_create();
    return pcb;
}

// add the PCB into ready_list according to the priority
void addtoQ(PCB* pcb){
   PCB* curr = List_first(ready_list);
   while(curr != NULL && curr->priority <= pcb->priority){
       curr = List_next(ready_list);
   }
   List_prev(ready_list);
   List_add(ready_list, pcb);
}



bool compare(void* it, void* p){

    int* pid = p;
    PCB* pcb = it;
    return pcb->pid == *pid;
}

// search the pid in list
PCB* search(List* list, int pid){
    List_first(list);

    return List_search(list,compare, &pid);
}
// remove sem from sem list.
void remove_sem(PCB* pcb){
   for(int i = 0; i < 5; ++i){
        Semaphore* s = sem[i];
        if(s == NULL || search(s->process, pcb->pid) == NULL){
            continue;
        }
        List_remove(s->process);
        s->val++;
    }
}
// remove certain pcb from sender_list
void remove_queue(List* list, PCB* pcb){
    PCB* copy = List_first(list);
    while(copy != NULL){
        if(search(copy->sender_list, pcb->pid) != NULL){
            List_remove(copy->sender_list);
        }
        copy = List_next(list);
    }
}
// free pcb
void free_pcb(PCB* pcb){
    PCB* copy = pcb;
    if(copy->sender_list != NULL){
        List_free(pcb->sender_list, NULL);
    }
    free(pcb);
}
// remove certain pcb
PCB* remove_psb(List* list, int pid){
    PCB* pcb = search(list, pid);
    if(pcb != NULL){
        List_remove(list);
    }
    return pcb;
}

// initialize semaphore
Semaphore* Semaphore_create(int semaphore, int val){
    Semaphore* se = malloc(sizeof(Semaphore));
    se->sid = semaphore;
    se->val = val;
    se->process = List_create();
    return se;
}
// free semaphore
void free_sem(Semaphore* se){
    if(se == NULL){
        return;
    }
    Semaphore* copy = se;
    if(copy->process != NULL){
        List_free(copy->process, NULL);
    }
    free(se);
}









// create pcb and add it into readylist
void Create(int pri){

    PCB* pcb = PCB_create(pri);
    addtoQ(pcb);
    printf("!!SUCCESS!! pid of created process is: %d.\n", pcb->pid);

}

// copy the running pcb, copy the first pcb in readylist if it is not initial
void Fork(){
   PCB* pcb = List_first(ready_list);
   if(pcb->pid != 0){
       PCB* copy = PCB_create(pcb->priority);
       addtoQ(copy);
       printf("!!SUCCESS!! pid of fork process is: %d.\n", copy->pid);
       return;
   }
   printf("!!Fail!! Initial cannot be fork.\n");

}

//remove the name pcb in ready_list
void Kill(int pid){
    // find pcb in ready and blocked list
    PCB* pcb = search(ready_list, pid);
    if(pcb != NULL){
        if(pcb->pid != 0){// remove from ready
            List_remove(ready_list);
        }
        else{
            if((List_count(ready_list) > 1 || List_count(block_list) > 0)){
                printf("!! Fail !! Init cannot be killed since other processes in the system\n");
            }
            else{ // remove initial
                printf("!! SUCCESS!! initial process kill.\n");
                running = false;
            }
            return;
        }
    }
    else{ // remove the block one
        pcb = search(block_list, pid);
        if(pcb == NULL){
            printf("!! Fail !! No such process: %d\n", pid);
            return;
        }
        List_remove(block_list);
    }
    // clean sem and sender_list 
    remove_sem(pcb);
    remove_queue(ready_list, pcb);
    remove_queue(block_list, pcb);
    printf("!! SUCCESS!! process %d kill.\n", pcb->pid);
    free_pcb(pcb);
}

// remove the running process
void Exit(){
    PCB* pcb = List_first(ready_list);

    if(pcb->pid == 0){

        if((List_count(ready_list) > 1 || List_count(block_list) > 0)){
            printf("!! Fail !! Init cannot be killed since other processes in the system\n");
        }
        else{
            printf("!! SUCCESS!! initial process kill.\n");
            running = false;
        }
        return;
    }
    List_remove(ready_list);
    remove_sem(pcb);
    remove_queue(ready_list, pcb);
    remove_queue(block_list, pcb);
    free_pcb(pcb);
    //set the new running process
    pcb = List_first(ready_list);
    printf("!! SUCCESS!! new process %d gets control of the CPU.\n", pcb->pid);
}

void Quantum(){
    //put the running process to ready queue
    PCB* pcb = List_first(ready_list);
    List_remove(ready_list);
    addtoQ(pcb);
    // get the new running process
    pcb = List_first(ready_list);
    printf("process %d control CPU", pcb->pid);
}

void Send(int pid, char* m){
    //avoid send to self and initial
    PCB* pcb = List_first(ready_list);
    if(pcb->pid == 0){
        printf("!! Fail!! Init cannot send message\n");
        return;
    }

    if(pcb->pid == pid){
        printf("!! Fail!! cannot send message to self\n");
        return;
    }
    //search the pid in ready or block queue
    PCB* copy = search(ready_list, pid);
    if(copy == NULL){
        copy = search(block_list, pid);
    }

    if(copy == NULL){
         printf("!! Fail!! no %d process.\n", pid);
        return;
    }
    //change the pcb to block and record the message to the pcb
    strcpy(pcb->storeMessage, m);
    pcb->state = BLOCK_SEND;
    // move to block list
    pcb = remove_psb(ready_list, pcb->pid);
    List_append(block_list, pcb);
    printf("!! SUCCESS!! Process %d is blocked now.\n",pcb->pid);
    
    // add the sender_list to process want to send
    List_append(copy->sender_list, pcb);
    // determine whether the process is ok to receive message
    if(copy->state == BLOCK_RECV){
        printf("Process %d receive message (%s) from process %d", copy->pid, pcb->storeMessage, pcb->pid);
        copy = remove_psb(block_list, copy->pid);
        copy->state = REDAY;
        addtoQ(copy);
    }
}


void Receive(){
    PCB* pcb = List_first(ready_list);
    if(pcb->pid == 0){
        printf("!! Fail!! Init cannot receive message\n");
        return;
    }

    // determine whether the process has receive the message
    if(List_count(pcb->sender_list) <= 0){
        // block to receive later
        pcb->state = BLOCK_RECV;
        List_remove(ready_list);
        List_append(block_list, pcb);

        printf("!! Success!! Process %d blocked now.\n", pcb->pid);
        return;
    }
    
    // when process has received the message
    PCB* copy = List_first(pcb->sender_list);
    printf("!!SUCCESS!! Already received message (%s) from process %d.\n",
        copy->storeMessage, copy->pid);
    return;
}


void Replay(int pid, char* word){

    PCB* pcb = List_first(ready_list);
    PCB* copy = search(pcb->sender_list, pid);

    if(copy == NULL){
        printf("!! Fail!! cannot replay process %d\n", pid);
        return;
    }
    // unblock sender and reply
    remove_psb(pcb->sender_list, pid);
    remove_psb(block_list, pid);
    // add process back to ready queue
    copy -> state = REDAY;
    addtoQ(copy);
    printf("Process %d receive reply (%s) from process %d.\n", pid, word, pcb->pid);
}

void New_Semaphore(int semaphore, int num){
    // check whether the semaphore has been used
    if(sem[semaphore] != NULL){
        printf("!! Fail!! semaphore id=%d has been used\n", semaphore);
        return;
    }
    sem_num++;
    // create the new semaphore
    sem[semaphore] = Semaphore_create(semaphore, num);

    printf("!! SUCCESS!! Semaphore id=%d has been created.\n", semaphore);
}

void SempahoreP(int sth){

    PCB* pcb = List_first(ready_list);
    // initial cannot be block
    if(pcb->pid == 0){
        printf("!! Fail!! Init cannot do semaphore\n");
        return;
    }
    //check whether exists
    Semaphore* copy = sem[sth];
    if(copy == NULL){
        printf("!! Fail!! No semaphore id=%d\n", sth);
        return;
    }
    // do sempahore p
    copy->val--;
    if(copy->val > 0){
        printf("!! SUCCESS!! Process %d still run.\n", pcb->pid);
        return;
    }
    // move from ready to block 
    pcb->state = BLOCK_SEM;
    List_remove(ready_list);
    List_append(block_list, pcb);
    // put pcb on semaphore's list
    List_append(copy->process, pcb);
    printf("!! SUCCESS!! Process %d is blocked now.\n", pcb->pid);
}

void SempahoreV(int semaphore){
    //determine whether the semaphore appear
    Semaphore* copy = sem[semaphore];
    if(copy == NULL){
        printf("!! Fail!! No semaphore id=%d\n", semaphore);
        return;
    }
    // do semaphore v
    copy->val++;
    if(copy->val > 0){
        printf("!! SUCCESS!!  no Process block on semaphore.\n");
        return;
    }
    // move from block to ready
    PCB* pcb = List_first(copy->process);
    List_remove(copy->process);
    pcb = remove_psb(block_list, pcb->pid);
    pcb->state = REDAY;
    addtoQ(pcb);
    printf("!! SUCCESS!! Process %d is ready now.\n", pcb->pid);
}

void Procinfo(int pid){
    // show the information of certain pcb
    PCB* pcb = search(ready_list, pid);
    if(pcb == NULL){
        pcb = search(block_list, pid);
    }

    if (pcb == NULL){
        printf("!!Fail!! No process %d.\n", pid);
        return;
    }
    printf("Process info \n");
    printf(" pid: %d\n",pcb->pid);
    printf(" priority: %d\n", pcb->priority);
    if(pcb == List_first(ready_list)){
        printf("Status: Running\n");
    }
    else if(pcb->state == REDAY){
        printf("Status: Ready\n");
    }
    else{
        printf("Status: Block\n");
    }
}

void Totalinfo(){
    // show the ready list 
    PCB* pcb;
    printf("Ready queue:\n");
    if(List_count(ready_list) > 1){
        List_first(ready_list);
        pcb = List_next(ready_list);
        while(pcb != NULL){
            printf(" pid is %d, priority is %d\n", pcb->pid, pcb->priority);
            pcb = List_next(ready_list);
        }
    }
    
    // show the block list
    printf("Block queue:\n");
    if(List_count(ready_list) != 0){
        pcb = List_first(block_list);
        while(pcb != NULL){
            printf(" pid is %d, priority is %d\n", pcb->pid, pcb->priority);
            pcb = List_next(block_list);
        }
    }

    // show the semaphores
    printf("Semaphores: \n");
    if(sem_num != 0){
        for(int i = 0; i < 5; i++){
            if(sem[i] == NULL){
                continue;
            }
            printf(" sid is %d, value is %d \n", sem[i]->sid, sem[i]->val);

        }
    }

    
}
