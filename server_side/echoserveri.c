#include "csapp.h"
#define NB_PROC 5
#define MAX_NAME_LEN 256

pid_t pid_fils[NB_PROC];

void echo(int connfd);

/*
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */

// TODO: Rework
void handler(int sig)
{
  for (int i = 0; i < NB_PROC; i++)
  { // Kill all child processes
    printf("fils mort {%d}\n", pid_fils[i]);
    kill(SIGINT, pid_fils[i]);
    waitpid(-1, NULL, 0);
  }
  exit(0);
}

/**
 * Creates NB_PROC children processes.
 *
 * @return The process ID of the parent process. Returns 0 for child processes.
 */
pid_t create_children()
{
  pid_t fils;
  int i = 0;

  while (i < NB_PROC)
  {
    //  Create a child process
    fils = Fork();

    if (fils == 0)
    { // Exit the function if the process is a child
      return 0;
    }
    else if (fils > 0)
    { // Store the process ID of the child process
      pid_fils[i] = fils;
    }
    else
    { // Handle errors
      perror("fork");
      exit(1);
    }

    i++;
  }

  return fils;
}

/**
 * Function to handle the communication with a client socket.
 *
 * @param listenfd The file descriptor of the listening socket.
 */
void socket_child(int listenfd)
{
  socklen_t clientlen;
  struct sockaddr_in clientaddr;
  char client_ip_string[INET_ADDRSTRLEN];
  char client_hostname[MAX_NAME_LEN];

  while (1)
  {
    // Accept a connection request from a client
    clientlen = sizeof(clientaddr);
    // TODO: Recheck slides - S'assurer pour tout les OS + Check result of Accept
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

/**
 * @brief Handles the child process for handling connections.
 *
 * This function restores the default SIGINT behavior, handles connections using the socket_child function,
 * and ensures that the child process exits after handling connections.
 *
 * @param listenfd The file descriptor of the listening socket.
 */
void handle_child_process(int listenfd)
{
  // Restore default SIGINT behavior
  Signal(SIGINT, SIG_DFL);
  // Handle connections
  socket_child(listenfd);
  exit(0); // Ensure child process exits after handling connections
}

int main()
{
  int listenfd, port;
  pid_t fils;

  port = 2121;

  // Create a listening descriptor
  listenfd = Open_listenfd(port);

  // New SIGINT behavior for the main process
  Signal(SIGINT, handler);

  // Create NB_PROC children processes and handle connections for children
  if ((fils = create_children()) == 0)
  {
    handle_child_process(listenfd);
  }

  while (1)
    ;
  exit(0);
}
