#include "csapp.h"
#include <time.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/**
 * Closes the connection and exits the program.
 *
 * @param connfd The file descriptor of the connection to be closed.
 */
void close_connection(int connfd)
{
  Close(connfd);
  exit(0);
}

int main(int argc, char **argv)
{
  int clientfd, port;
  char *host, buf[MAXLINE];
  rio_t rio;

  if (argc != 2) // Check if the user has provided the host
  {
    fprintf(stderr, "usage: %s <host>\n", argv[0]);
    exit(0);
  }

  host = argv[1];
  port = 2121;

  // Connect to the server
  clientfd = Open_clientfd(host, port);
  // Initialize the rio_t structure
  Rio_readinitb(&rio, clientfd);

  printf("Connected to %s\n", host);

  // Waiting client instructions
  while (1)
  {

    // Print the prompt
    printf("ftp> ");

    // Read the input from the user
    if (Fgets(buf, MAXLINE, stdin) == NULL)
    {
      close_connection(clientfd);
    }

    Rio_writen(clientfd, buf, strlen(buf));

    // Client wants to get a file
    if (strncmp(buf, "get ", 4) == 0)
    {
      char filename[MAXLINE];
      sscanf(buf + 4, "%s", filename);
      char filebuf[MAXLINE];

      long file_size;
      Rio_readnb(&rio, &file_size, sizeof(long)); // Read the size of the file

      // DONE : dont create the file if the server sends an error message (file not found)
      if (file_size == -1)
      {
        printf("Error: File not found on server.\n");
        continue;
      }
      else
      {
        FILE *file = Fopen(filename, "wb");

        clock_t start_time = clock(); // Save the start time

        if (file != NULL)
        {
          ssize_t bytes_read;
          size_t total_bytes_read = 0;
          // Read the file until the total bytes read is equal to the file size
          while (total_bytes_read < file_size)
          {
            // Read the file in chunks of MAXLINE bytes or less
            bytes_read = Rio_readnb(&rio, filebuf, MIN(file_size - total_bytes_read, MAXLINE));
            // Write the chunk to the file
            Fwrite(filebuf, 1, bytes_read, file);
            // Update the total bytes read
            total_bytes_read += bytes_read;
          }
          Fclose(file);
          printf("Transfer successfully complete.\n");

          clock_t end_time = clock();                                      // Save the end time
          double temps = (double)(end_time - start_time) / CLOCKS_PER_SEC; // Calculate the elapsed time
          double vitesse = (total_bytes_read / 1024.0) / temps;            // Calculate average speed in Kbytes/s
          printf("%ld bytes received in %.4f seconds (%.2f Kbytes/s)\n", total_bytes_read, temps, vitesse);
        }
        else
        {
          printf("Error: Failed to open file for writing.\n");
        }
      }
    }
    // Client wants to quit
    else if (strncmp(buf, "bye\n", 4) == 0)
    {
      printf("Connection closed.\n");
      Close(clientfd);
      exit(0);
    }
  }
  close_connection(clientfd);
}
