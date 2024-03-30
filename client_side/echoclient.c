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

/**
 * Receive a file from the server and save it locally.
 *
 * @param rio The rio_t object for reading from the server.
 * @param connfd The file descriptor for the connection to the server.
 * @param filename The name of the file to be saved locally.
 */
void get_client(rio_t rio, int connfd, char *filename)
{
    long file_size;
    ssize_t bytes_read;
    size_t total_bytes_read = 0;
    char filebuf[MAXLINE];

    // Read the size of the file from the server
    Rio_readnb(&rio, &file_size, sizeof(long));

    // Stop here if an error occurred from the server (File not found)
    if (file_size == -1)
    {
        fprintf(stderr, "Error: File not found on server.\n");
        return;
    }

    // Open the file in write mode (create it if it doesn't exist)
    FILE *file = Fopen(filename, "wb");

    // Check if the file was opened successfully
    if (file == NULL)
    {
        fprintf(stderr, "Error: Failed to open file for writing.\n");
        return;
    }

    // Save the start time of the transfer
    clock_t start_time = clock();

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

    // Save the end time
    clock_t end_time = clock();

    // Compute the time and average speed
    double time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    double speed = (total_bytes_read / 1024.0) / time;
    printf("%ld bytes received in %.4f seconds (%.2f Kbytes/s)\n", total_bytes_read, time, speed);
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
            // If the user presses Ctrl+D (EOF), close the connection and exit
            close_connection(clientfd);
        }

        // Send the input to the server
        Rio_writen(clientfd, buf, strlen(buf));

        if (strncmp(buf, "get ", 4) == 0)
        { // The client wants to fetch a file
            char filename[MAXLINE];
            sscanf(buf + 4, "%s", filename);
            get_client(rio, clientfd, filename);
        }

        else if (strncmp(buf, "bye\n", 4) == 0)
        { // The client wants to disconnect
            printf("Connection closed.\n");
            close_connection(clientfd);
        }
    }

    close_connection(clientfd);
}
