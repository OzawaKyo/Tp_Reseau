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

      FILE *file = Fopen(filename, "rb");

      if (file != NULL)
      { // If file exists and can be opened
        char filebuf[MAXLINE];
        ssize_t bytes_read;

        // TODO: fstat pour avoir la taille du fichier // DONE
          long file_size;
          struct stat file_stat;
          if (fstat(fileno(file), &file_stat) == 0) {
            file_size = file_stat.st_size;
          }

        // Read the file
        while ((bytes_read = Fread(filebuf, 1, MAXLINE, file)) > 0)
        {
          printf("Line: %s\n", filebuf);

          printf("Server sending %ld bytes\n", bytes_read);
          Rio_writen(connfd, filebuf, bytes_read); // Send the file
        }
        Fclose(file);
        printf("File sent and closed: %s\n", filename);
      }
      else
      {
        char *msg = "File not found\n";
        printf("File not found: %s\n", filename);
        Rio_writen(connfd, msg, strlen(msg));
      }
    }
    else
    {
      printf("Server received %u bytes\n", (unsigned int)n);
      Rio_writen(connfd, buf, n);
    }
  }
}
