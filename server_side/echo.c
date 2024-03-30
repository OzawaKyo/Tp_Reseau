#include "csapp.h"
#define SMALL_BUF 256 // MAXLINE is too large for user requests (8192B -> 256B)

/**
 * @brief Sends the contents of a file to the client.
 *
 * This function takes a file name and a connection file descriptor as input.
 * It opens the file in read mode and sends its contents to the client in chunks of MAXLINE bytes.
 * If an error occurs it sends a negative value to the client.
 * -1: File not found
 * -2: Permission denied
 * -3: Error opening file
 *
 * @param connfd The connection file descriptor.
 * @param filename The name of the file to be sent.
 */
void get_server(int connfd, char *filename)
{
    long file_size;
    long start_pos;
    char filebuf[MAXLINE];
    ssize_t bytes_read;
    struct stat file_stat;

    printf("Client requested file %s\n", filename);

    // Open the file in read mode
    FILE *file = fopen(filename, "rb");

    // An error occurred while opening the file (file not found)
    if (file == NULL)
    {
        // if permission denied
        if (errno == EACCES)
        {
            file_size = -2;
            rio_writen(connfd, &file_size, sizeof(long));
            printf("Permission denied: %s\n", filename);
            return;
        }
        else if (errno == ENOENT)
        {
            // Send the size of the file as -1 to indicate an error
            file_size = -1;
            rio_writen(connfd, &file_size, sizeof(long));
            printf("File not found: %s\n", filename);
            return;
        }
        else
        {
            file_size = -3;
            rio_writen(connfd, &file_size, sizeof(long));
            printf("Error opening file: %s\n", filename);
            return;
        }
    }

    // Get the size of the file
    if (fstat(fileno(file), &file_stat) == 0)
        file_size = file_stat.st_size;
    else
        file_size = -1;

    // Send the size of the file to the client
    rio_writen(connfd, &file_size, sizeof(long));

    // Get the desired start position from the client
    rio_readn(connfd, &start_pos, sizeof(long));
    // Set the file position to the desired start position
    fseek(file, start_pos, SEEK_SET);

    // Send the file to the client in chunks of 8192 bytes
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
void bye_server(int connfd)
{
    printf("Client disconnected\n");
    Close(connfd);
}

/**
 * @brief Echoes the client's requests.
 *
 * This function reads lines from the client's connection descriptor and processes them accordingly.
 * If the client sends a "get" request, it fetches the specified file.
 * If the client sends a "bye" request, it disconnects from the server.
 * Otherwise, it prints the number of bytes received from the client.
 *
 * @param connfd The connection descriptor for the client.
 */
void echo(int connfd)
{
    size_t n;
    char buf[SMALL_BUF];
    rio_t rio;

    // Initializes a read buffer for the descriptor connfd
    Rio_readinitb(&rio, connfd);

    // Reads a line from the descriptor connfd (The clients request)
    while ((n = Rio_readlineb(&rio, buf, SMALL_BUF)) != 0)
    {

        if (strncmp(buf, "get ", 4) == 0)
        { // The client wants to fetch a file
            char filename[SMALL_BUF];
            sscanf(buf + 4, "%s", filename);
            get_server(connfd, filename);
        }
        else if (strncmp(buf, "bye\n", 4) == 0)
        { // The client wants to disconnect
            bye_server(connfd);
            return;
        }
        else
        { // The client sent a random message
            printf("Server received %u bytes\n", (unsigned int)n);
        }
    }
}
