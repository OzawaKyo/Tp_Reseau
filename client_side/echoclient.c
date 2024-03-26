#include "csapp.h"
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int main(int argc, char **argv)
{
  int clientfd, port;
  char *host, buf[MAXLINE];
  rio_t rio;

  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <host>\n", argv[0]);
    exit(0);
  }
  host = argv[1];
  port = 2121;

  // Connect to server
  clientfd = Open_clientfd(host, port);
  Rio_readinitb(&rio, clientfd);

  // Waiting client instructions
  while (Fgets(buf, MAXLINE, stdin) != NULL)
  {

    Rio_writen(clientfd, buf, strlen(buf));

    // Client wants to get a file
    if (strncmp(buf, "get ", 4) == 0)
    {
      char filename[MAXLINE];
      sscanf(buf + 4, "%s", filename);
      char filebuf[MAXLINE];

      long file_size;
      Rio_readnb(&rio, &file_size, sizeof(long)); // Read the size of the file

      FILE *file = Fopen(filename, "wb");
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
        printf("File received and saved: %s\n", filename);
      }
      else
      {
        printf("Error: Failed to open file for writing.\n");
      }
    }
  }
  Close(clientfd);
  exit(0);
}
