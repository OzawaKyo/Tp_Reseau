#include "csapp.h"

/**
 * @brief Sends the contents of a file to the client.
 *
 * This function takes a file name and a connection file descriptor as input.
 * It opens the file in read mode and sends its contents to the client in chunks of MAXLINE bytes.
 * If the file is not found, it sends the size of the file as -1 to indicate an error.
 *
 * @param connfd The connection file descriptor.
 * @param filename The name of the file to be sent.
 */
void get(int connfd, char *filename)
{
  long file_size;
  char filebuf[MAXLINE];
  ssize_t bytes_read;
  struct stat file_stat;

  printf("Client requested file %s\n", filename);

  // Open the file in read mode
  FILE *file = Fopen(filename, "rb");

  // An error occurred while opening the file (file not found)
  if (file == NULL)
  {
    // Send the size of the file as -1 to indicate an error
    file_size = -1;
    rio_writen(connfd, &file_size, sizeof(long));
    printf("File not found: %s\n", filename);
    return;
  }

  // Get the size of the file
  if (fstat(fileno(file), &file_stat) == 0)
    file_size = file_stat.st_size;
  else
    file_size = -1;

  // Send the size of the file to the client
  rio_writen(connfd, &file_size, sizeof(long));

  // Send the file to the client in chunks of MAXLINE bytes
  while ((bytes_read = Fread(filebuf, 1, MAXLINE, file)) > 0)
  {
    printf("Server sending %ld bytes\n", bytes_read);
    // Send the chunk to the client
    Rio_writen(connfd, filebuf, bytes_read);
  }
  Fclose(file);
  printf("File sent and closed: %s\n", filename);
}

/**
 * @brief Closes the connection with the client.
 *
 * @param connfd The file descriptor of the connection with the client.
 */
void bye(int connfd)
{
  printf("Client disconnected\n");
  Close(connfd);
}

void echo(int connfd)
{
  size_t n;
  char buf[MAXLINE];
  rio_t rio;

  // Initializes a read buffer for the descriptor connfd
  Rio_readinitb(&rio, connfd); // TODO: WHY ? initialise un tampon de lecture pour le descripteur connfd

  // Reads a line from the descriptor connfd (The clients request)
  while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) // TODO: Maxline too large for small requests
  {

    if (strncmp(buf, "get ", 4) == 0)
    { // The client wants to fetch a file
      char filename[MAXLINE];
      sscanf(buf + 4, "%s", filename);
      get(connfd, filename);
    }
    else if (strncmp(buf, "bye\n", 4) == 0)
    { // The client wants to disconnect
      bye(connfd);
      return;
    }
    else
    { // The client sent a random message
      printf("Server received %u bytes\n", (unsigned int)n);
    }
  }
}
