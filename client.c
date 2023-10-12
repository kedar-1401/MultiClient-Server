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

void str_overwrite_stdout();

void str_overwrite_stdout() {
    printf("\r%s", "> ");
    fflush(stdout);
}
// volatile sig_atomic_t flag = 0;
int sockfd = 0;
char nickname[30];

// void catch_ctrl_c_and_exit(int sig) {
//     flag = 1;
// }

void reading() {
    char receiveMessage[1024];
    while (1) {
        int receive = read(sockfd, receiveMessage, 1024);
        if (receive > 0) {
            printf("\r%s\n", receiveMessage);
            str_overwrite_stdout();
        } else if (receive == 0) {
            break;
        }
    }
}

void writing() {
    char message[1024];
    while (1) {
        str_overwrite_stdout();
        while (fgets(message, 1024, stdin) != NULL) {
       
        for (int i = 0; i < 1024; i++) { 
            if (message[i] == '\n') {
                message[i] = '\0';
                break;
            }
        }

        if (strlen(message) == 0) {
                str_overwrite_stdout();
            } else {
                break;
            }
        }
        write(sockfd, message, 1024);
        if (strcmp(message, "exit") == 0) {
            break;
        }
    }
    //catch_ctrl_c_and_exit(2);
}
int main()
{
    //signal(SIGINT, catch_ctrl_c_and_exit);
    printf("Please enter your name: ");
    if (fgets(nickname, 30, stdin) != NULL) {
        for (int i = 0; i < 1024; i++) { 
            if (nickname[i] == '\n') {
                nickname[i] = '\0';
                break;
            }
        }
    }
    sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if (sockfd == -1) {
        printf("Fail to create a socket.");
        exit(1);
    }
    struct sockaddr_in servaddr, cliaddr;
    bzero(&cliaddr,sizeof(cliaddr));
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(8888);

 
    int err = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (err == -1) {
        printf("Connection to Server error!\n");
        exit(1);
    }
    printf("Connect to Server: %s:%d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));
    write(sockfd,nickname,30);

    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, (void *) writing, NULL) != 0) {
        printf ("Create pthread error!\n");
        exit(1);
    }
    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, (void *) reading, NULL) != 0) {
        printf ("Create pthread error!\n");
        exit(1);
    }
    while (1) {
        // if(flag) {
           
        //     break;
        // }
    }
    close(sockfd);
    return 0;
}
