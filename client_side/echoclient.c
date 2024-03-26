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

    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);

    // Waiting client instructions
    while (Fgets(buf, MAXLINE, stdin) != NULL)
    {

        Rio_writen(clientfd, buf, strlen(buf));

        // Wants to get a file
        if (strncmp(buf, "get ", 4) == 0)
        {
            char filename[MAXLINE];
            sscanf(buf + 4, "%s", filename);
            char filebuf[MAXLINE];

            long file_size;
            Rio_readnb(&rio, &file_size, sizeof(long));

            FILE *file = Fopen(filename, "wb");
            if (file != NULL)
            {
                ssize_t bytes_read;
                size_t total_bytes_read = 0;
                while (total_bytes_read < file_size)
                {
                    printf("Client receiving %ld bytes\n", file_size - total_bytes_read);
                    printf("a\n");
                    bytes_read = Rio_readnb(&rio, filebuf, MIN(file_size - total_bytes_read, MAXLINE));
                    printf("b\n");
                    Fwrite(filebuf, 1, bytes_read, file);
                    printf("c\n");
                    total_bytes_read += bytes_read;
                    printf("d\n");
                    Fclose(file);
                    printf("File received and saved: %s\n", filename);
                }
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
