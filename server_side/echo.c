/*
 * echo - read and echo text lines until client closes connection
 */
#include "csapp.h"

void echo(int connfd)
{
  size_t n;
  char buf[MAXLINE];
  rio_t rio;

  // Initializes a read buffer for the descriptor connfd
  Rio_readinitb(&rio, connfd); // TODO: WHY ?

  // Reads a line from the descriptor connfd (The clients request)
  while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) // TODO: Maxline too large
  {
    // If the client wants to get a file
    if (strncmp(buf, "get ", 4) == 0)
    {
      char filename[MAXLINE];
      sscanf(buf + 4, "%s", filename);
      printf("Server received request for file %s\n", filename);

      FILE *file = fopen(filename, "rb");
      long file_size;

      if (file != NULL)
      { // If file exists and can be opened
        char filebuf[MAXLINE];
        ssize_t bytes_read;

        // DONE : fstat pour avoir la taille du fichier
          struct stat file_stat;
          if (fstat(fileno(file), &file_stat) == 0) {
            file_size = file_stat.st_size;
          }else{
            file_size = -1;
          }

        rio_writen(connfd, &file_size, sizeof(long)); // Send the size of the file

        // Read the file
        while ((bytes_read = Fread(filebuf, 1, MAXLINE, file)) > 0)
        {
          printf("Server sending %ld bytes\n", bytes_read);
          Rio_writen(connfd, filebuf, bytes_read); // Send the file
        }
        Fclose(file);
        printf("File sent and closed: %s\n", filename);
      }
      else
      {
        file_size = -1;
        rio_writen(connfd, &file_size, sizeof(long)); // Send the size of the file as -1 to indicate an error
        printf("File not found: %s\n", filename);
      }
    }
    else
    {
      printf("Server received %u bytes\n", (unsigned int)n);
    }
  }
}
