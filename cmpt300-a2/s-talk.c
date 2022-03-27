/*
 Assignment 2
 Qingrui Li 301400099
 Kaixin Yang 301393040
 */

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include "list.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MSG_MAX_LEN 1024

// create the four thread and mutex
pthread_t type_t;
pthread_t send_t;
pthread_t receive_t;
pthread_t print_t;
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
int sockfd;

//address for sockect
struct sockaddr_storage sin_add;
struct addrinfo hints;
struct addrinfo *servinfo;
struct addrinfo *p;
struct addrinfo hints1;
struct addrinfo *servinfo1;

void* type_mess(void *word);
void* send_mess(void *word);
void* receive_mess(void* word);
void* print_mess(void* word);

void* Free_item(void* pItem);

int main(int argc, char *argv[]){
    // use the socket to conncet with another computer
    int status;
    //the information for the sender
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if((status = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0){
        fprintf(stderr, "getaddrinfo: 1");
        exit(1);
    }

    for(p = servinfo; p != NULL; p = p->ai_next){

        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(sockfd == -1){
            perror("socket error");
            continue;
        }

        if(bind(sockfd, p->ai_addr,p->ai_addrlen) == -1){
            close(sockfd);
            perror("bind error");
            continue;
        }
        break;
    }


    freeaddrinfo(servinfo);
    if(p == NULL){
        fprintf(stderr,"failed to bind socket\n");
        exit(1);
    }
    // information for receiver 
    memset(&hints1, 0, sizeof(hints1));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if((status = getaddrinfo(argv[2], argv[3], &hints, &servinfo1)) != 0){
        fprintf(stderr, "getaddrinfo: 2");
        exit(1);
    }
   
   //intialize the list and four pthread
    List *p1 = List_create();
    List *p2 = List_create();

    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    printf("Starting Chat........\n");

    //create threads
    pthread_create(&type_t, NULL, type_mess, p1);
    pthread_create(&send_t, NULL, send_mess, p1);
    pthread_create(&receive_t, NULL, receive_mess, p2);
    pthread_create(&print_t, NULL, print_mess, p2);

    //join threads
    pthread_join(type_t, NULL);
    pthread_join(send_t, NULL);
    pthread_join(receive_t, NULL);
    pthread_join(print_t, NULL);

    // free all everything
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);

    close(sockfd);

    List_free(p1, NULL);
    List_free(p2, NULL);
    return 0;
}

// type the message on screen
void * type_mess(void * word){

    List *l = (List*) word;
    char sen[MSG_MAX_LEN];

    while(1){
        // read the message from screen
        read(0, sen, MSG_MAX_LEN);

        if( *sen != '!'){
            pthread_mutex_lock(&mutex1);
            List_add(l, sen);
            pthread_mutex_unlock(&mutex1);
        }
        else{ // stop the whole chat when appear single !
            pthread_mutex_lock(&mutex1);
            List_add(l, sen);
            pthread_mutex_unlock(&mutex1);
            pthread_cancel(send_t);
            pthread_cancel(print_t);
            close(sockfd);
            exit(1);
        }
    }
    pthread_exit(0);
}

// send message to other people 
void *send_mess(void *word){
    char *sen;
    List *l = (List*) word;
    while(1){

        if( List_count(l) > 0){
            pthread_mutex_lock(&mutex1);
            sen = List_first(l);
            List_remove(l);
            pthread_mutex_unlock(&mutex1);
            if(sendto(sockfd, sen, strlen(sen), 0, servinfo1->ai_addr, servinfo1->ai_addrlen) == -1){
                exit(1);
            }
        }
    }
    pthread_exit(0);
}

//receive message from sender
void* receive_mess(void *word){
    char sen[MSG_MAX_LEN];
    List *l = (List*) word;
    socklen_t sin_len = sizeof(sin_add);
    while(1){
        // receive message from other computer
        if(recvfrom(sockfd, sen, MSG_MAX_LEN-1, 0, (struct sockaddr*)&sin_add, &sin_len) == -1){
                exit(1);
            }
        pthread_mutex_lock(&mutex2);
        List_add(l, sen);
        pthread_mutex_unlock(&mutex2);
    }
    pthread_exit(0);
}

// print message on screen
void* print_mess(void *word){
    char *sen;
    List *l = (List*) word;

    while(1){

        if( List_count(l) > 0){
            pthread_mutex_lock(&mutex2);
            sen = List_first(l);
            List_remove(l);
            pthread_mutex_unlock(&mutex2);
            int i = 0;
            while(sen[i] != '\n'){
                // write the massage
                write(1,&sen[i],1);
                i++;
            }
            printf("\n");
           if( *sen == '!'){ // show it on screen, stop the chat when single ! appears.
                printf("Chat End!\n");
                pthread_cancel(send_t);
                pthread_cancel(receive_t);
                pthread_cancel(type_t);
                close(sockfd);
                exit(1);
            }
        }
    }
    pthread_exit(0);
}
