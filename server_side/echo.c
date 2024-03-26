/*
 * echo - read and echo text lines until client closes connection
 */
#include "csapp.h"

void echo(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0)
    {
        if (strncmp(buf, "get ", 4) == 0)
        {
            char filename[MAXLINE];
            sscanf(buf + 4, "%s", filename); 
            printf("Server received request for file %s\n", filename);

            FILE *file = Fopen(filename, "rb");
            if (file != NULL)
            {
                char filebuf[MAXLINE];
                ssize_t bytes_read;
                
                fseek(file, 0, SEEK_END);
                long file_size = ftell(file);
                rewind(file);
                rio_writen(connfd, &file_size, sizeof(long));

                while ((bytes_read = Fread(filebuf, 1, MAXLINE, file)) > 0)
                {
                    printf("Line: %s\n", filebuf); // ICI

                    printf("Server sending %ld bytes\n", bytes_read);
                    Rio_writen(connfd, filebuf, bytes_read);
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
