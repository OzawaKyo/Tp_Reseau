#include "csapp.h"

int main(int argc, char **argv) {
    int clientfd, port;
    char *host, buf[MAXLINE];
    rio_t rio;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <host>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = 2121;

    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);

    while (Fgets(buf, MAXLINE, stdin) != NULL) {
        Rio_writen(clientfd, buf, strlen(buf));
        if (strncmp(buf, "get ", 4) == 0) {
            char filename[MAXLINE];
            sscanf(buf + 4, "%s", filename);
            char filebuf[MAXLINE];
            if (Rio_readlineb(&rio, buf, MAXLINE) > 0 && strncmp(buf, "File not found", 14) != 0) {
                FILE* file = Fopen(filename, "wb");
                if (file != NULL) {
                    printf("Receiving file: %s\n", filename);
                    ssize_t bytes_read;
                    while ((bytes_read = Rio_readnb(&rio, filebuf, MAXLINE)) > 0) {
                        printf("Received %ld bytes\n", bytes_read);
                        Fwrite(filebuf, 1, bytes_read, file);
                    }
                    Fclose(file);
                    printf("File received and saved: %s\n", filename);
                } else {
                    printf("Error: Failed to open file for writing.\n");
                }
            }else{
                printf("Error: File not found.\n");
            }
        }
    }
    Close(clientfd);
    exit(0);
}

