#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

typedef struct ClientNode {
    int data;
    struct ClientNode* prev;
    struct ClientNode* link;
    char ip[16];
    char name[31];
} C;

C *newNode(int sockfd, char* ip) {
    C *np = (C *)malloc( sizeof(C) );
    np->data = sockfd;
    np->prev = NULL;
    np->link = NULL;
    strncpy(np->ip, ip, 16);
    strncpy(np->name, "NULL", 5);
    return np;
}

int serverfd = 0, clientfd = 0;
C *root, *now;

void clientsSend(C *np, char tmp_buffer[]) {
    C *tmp = root->link;
    while (tmp != NULL) {
        if (np->data != tmp->data) {
            write(tmp->data, tmp_buffer, 1024);
        }
        tmp = tmp->link;
    }
}

void client_handler(void *p_client) {
    int leave_flag = 0;
    char nickname[31];
    char readMsg[101];
    char writeMsg[201];
    C *np = (C *)p_client;

        // if(recv(np->data, nickname, 31,0)<=0){
        //     return;
        // }
        if(read(np->data, nickname, 31)<=0){
            return;
        }
        strncpy(np->name, nickname, 31);
        // printf("Someone connected\n");
        // sprintf(writeMsg, "%s connected", np->name);
        printf("%s Connected\n", np->name);
        sprintf(writeMsg, "%s connected", np->name);
        clientsSend(np, writeMsg);
    
    while (1) {
        // int receive = recv(np->data, readMsg, 101,0 );
        int receive=read(np->data, readMsg, 101);
         if (receive == 0 || strcmp(readMsg, "exit") == 0) {
            break;
        }
        // if (leave_flag) {
        //     break;
        // }
        
        if (receive > 0) {
            if (strlen(readMsg) == 0) {
                continue;
            }
            sprintf(writeMsg, "%s：%s ", np->name, readMsg);
         }
         // else if (receive == 0 || strcmp(readMsg, "exit") == 0) {
        //     printf("%s(%s)(%d) leave the chatroom.\n", np->name, np->ip, np->data);
        //     sprintf(writeMsg, "%s(%s) leave the chatroom.", np->name, np->ip);
        //     leave_flag = 1;
        //}
        //  else {
            
        //     leave_flag = 1;
        // }
        clientsSend(np, writeMsg);
    }

  
    close(np->data);
    if (np == now) {
        now = np->prev;
        now->link = NULL;
    } else { 
        np->prev->link = np->link;
        np->link->prev = np->prev;
    }
    free(np);
}

int main()
{
    serverfd = socket(AF_INET , SOCK_STREAM , 0);
    if (serverfd == -1) {
        printf("Fail to create a socket.");
        exit(1);
    }

    struct sockaddr_in server_info, client_info;
    
    bzero(&server_info,sizeof(server_info));
    bzero(&client_info,sizeof(client_info));
    server_info.sin_family = PF_INET;
    server_info.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_info.sin_port = htons(8888);
    bind(serverfd, (struct sockaddr *)&server_info, sizeof(server_info));
    listen(serverfd, 5);
    printf("Start Server on: %s:%d\n", inet_ntoa(server_info.sin_addr), ntohs(server_info.sin_port));

    root = newNode(serverfd, inet_ntoa(server_info.sin_addr));
    now = root;

    while (1) {
        clientfd= accept(serverfd, (struct sockaddr*) &client_info, (socklen_t*) &client_info);
        printf("Client %s:%d come in.\n", inet_ntoa(client_info.sin_addr), ntohs(client_info.sin_port));

        C *c = newNode(clientfd, inet_ntoa(client_info.sin_addr));
        c->prev = now;
        now->link = c;
        now = c;

        pthread_t id;
        if (pthread_create(&id, NULL, (void *)client_handler, (void *)c) != 0) {
            perror("Create pthread error!\n");
            exit(1);
        }
    }
    return 0;
}
