/*
 * echoserveri.c - An iterative echo server
 */

#include "csapp.h"
#define NB_PROC 5
#define MAX_NAME_LEN 256

pid_t pid_fils[NB_PROC];

void echo(int connfd);

/*
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */

void handler(int sig)
{
  for (int i = 0; i < 5; i++)
  {
    printf("fils mort {%d}\n", pid_fils[i]);
    kill(SIGINT, pid_fils[i]);
    waitpid(-1, NULL, 0);
  }
  exit(0);
}

void socket_fils(int listenfd)
{
  socklen_t clientlen;
  struct sockaddr_in clientaddr;
  char client_ip_string[INET_ADDRSTRLEN];
  char client_hostname[MAX_NAME_LEN];

  while (1)
  {
    clientlen = sizeof(clientaddr);
    int connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

    /* determine the name of the client */
    Getnameinfo((SA *)&clientaddr, clientlen,
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

int main()
{
  int listenfd, port;
  pid_t fils;
  int i = 0;

  port = 2121;

  listenfd = Open_listenfd(port);
  Signal(SIGINT, handler);

  while (i < NB_PROC)
  {
    fils = Fork();
    if (fils == 0)
    { // child process
      Signal(SIGINT, SIG_DFL);
      socket_fils(listenfd);
      exit(0); // Ensure child process exits after handling connections
    }
    else if (fils > 0)
    { // parent process
      pid_fils[i] = fils;
      i++;
    }
    else
    {
      perror("fork");
      exit(0);
    }
  }
  while (1)
    ;
  exit(0);
}
