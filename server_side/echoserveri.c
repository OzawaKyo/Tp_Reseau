/*
 * echoserveri.c - An iterative echo server
 */

#include "csapp.h"

#define MAX_NAME_LEN 256

pid_t pid_fils[5];

void echo(int connfd);

/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */

void handler(int sig) {
    for (int i = 0; i < 5; i++) {
        printf("fils mort {%d}\n", pid_fils[i]);
        kill(SIGINT, pid_fils[i]);
        waitpid(-1, NULL, 0);
    }
    exit(0);
}

void socket_fils(int listenfd, socklen_t clientlen, struct sockaddr_in clientaddr, char *client_ip_string, char *client_hostname) {
    int connfd;
    while (1) {

        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

        /* determine the name of the client */
        Getnameinfo((SA *) &clientaddr, clientlen,
                    client_hostname, MAX_NAME_LEN, 0, 0, 0);

        /* determine the textual representation of the client's IP address */
        Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                INET_ADDRSTRLEN);

        printf("server connected to %s (%s)\n", client_hostname,
            client_ip_string);

        echo(connfd);
        Close(connfd);
    }
}

int main(int argc, char **argv)
{
    int listenfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);
    
    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);
    pid_t fils;
    int i = 0;
    Signal(SIGINT, handler);
    while (i < 5) {
        fils = Fork();
        if (fils == 0) { //fils
            Signal(SIGINT, SIG_DFL);
            socket_fils(listenfd,clientlen,clientaddr,client_ip_string,client_hostname);
        } else if (fils > 0) { //père
            pid_fils[i] = fils;
            i++;
        } else {
            perror("fork");
            exit(0);
        }
    }
    while (1);
    exit(0);
}

