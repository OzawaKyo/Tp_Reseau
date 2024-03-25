/*
 * echo - read and echo text lines until client closes connection
 */
#include "csapp.h"

// void echo(int connfd)
// {
//     size_t n;
//     char buf[MAXLINE];
//     rio_t rio;

//     Rio_readinitb(&rio, connfd);
//     while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
//         printf("server received %u bytes\n", (unsigned int)n);
//         Rio_writen(connfd, buf, n);
//     }
// }
void echo(int connfd) {
    size_t n;
    char buf[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        if (strncmp(buf, "get ", 4) == 0) {
            char* filename = buf + 4;
            printf("Server received request for file %s\n", filename);
            //create the file with the name filename

            FILE* file = Fopen(filename, "r");
            if (file != NULL) {
                char filebuf[MAXLINE];
                ssize_t bytes_read;
                while ((bytes_read = Fread(filebuf, 1, MAXLINE, file)) > 0) {
                    printf("Server sending %ld bytes\n", bytes_read);
                    Rio_writen(connfd, filebuf, bytes_read);
                }
                Fclose(file);
            } else {
                char* msg = "File not found\n";
                printf("Fine not found\n");
                Rio_writen(connfd, msg, strlen(msg));
            }
        } else {
            printf("Server received %u bytes\n", (unsigned int)n);
            Rio_writen(connfd, buf, n);
        }
    }
}
