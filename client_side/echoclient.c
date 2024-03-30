#include "csapp.h"
#include <time.h>

#define SMALL_BUF 256  // MAXLINE is too large for user requests (8192B -> 256B)
#define SLOW_WRITING 1 // Set to 1 to slow down the write operation (testing purposes)

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/**
 * @brief Displays a progress bar based on the given progress.
 *
 * @param progress The current progress value (0-100)
 */
void progress_bar(int progress)
{
    int WIDTH = 100;

    printf("\r[");
    for (int i = 0; i < WIDTH; ++i)
    {
        if (i < progress)
            printf("#");
        else
            printf(" ");
    }
    printf("] %d%%", progress * 100 / WIDTH);
    // Print immediately
    fflush(stdout);
    if (progress == 100)
        printf("\n");
}

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
    size_t bytes_already_written = 0;
    char filebuf[MAXLINE];
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 10 * 1000000; // 100 milliseconds

    // Read the size of the file from the server
    Rio_readnb(&rio, &file_size, sizeof(long));

    // Stop here if an error occurred from the server (File not found)
    if (file_size == -1)
    {
        fprintf(stderr, "Error: File not found on server.\n");
        return;
    }
    if (file_size == -2)
    {
        fprintf(stderr, "Error: Permission denied for this file.\n");
        return;
    }

    // Check if the file already exists, if yes get the file size
    FILE *file_exists = fopen(filename, "rb");
    if (file_exists != NULL)
    {
        struct stat file_stat;
        // File size
        if (fstat(fileno(file_exists), &file_stat) == 0)
        {
            bytes_already_written = file_stat.st_size;
        }
        Fclose(file_exists);
        // Set the total bytes read to the file size because these bytes are already written
    }

    // Send the desired start position to the server
    Rio_writen(connfd, &bytes_already_written, sizeof(long));

    // Open the file in append mode (create it if it doesn't exist)
    FILE *file = Fopen(filename, "ab");

    // Check if the file was opened successfully
    if (file == NULL)
    {
        fprintf(stderr, "Error: Failed to open file for writing.\n");
        return;
    }

    // Save the start time of the transfer
    clock_t start_time = clock();

    // Read the file until the total bytes read is equal to the file size
    while ((total_bytes_read + bytes_already_written) < file_size)
    {
        // Read the file in chunks of MAXLINE bytes or less
        bytes_read = Rio_readnb(&rio, filebuf, MIN(file_size - (total_bytes_read + bytes_already_written), MAXLINE));
        // Write the chunk to the file
        Fwrite(filebuf, 1, bytes_read, file);
        // Update the total bytes read
        total_bytes_read += bytes_read;

        // Display the progress bar
        progress_bar((total_bytes_read + bytes_already_written) * 100 / file_size);

        // Set SLOW_WRITING to 1 to slow down the write operation (testing purposes)
        if (SLOW_WRITING)
            nanosleep(&ts, NULL);
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
    char *host, buf[SMALL_BUF];
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
        if (Fgets(buf, SMALL_BUF, stdin) == NULL)
        {
            // If the user presses Ctrl+D (EOF), close the connection and exit
            close_connection(clientfd);
        }

        // Send the input to the server
        Rio_writen(clientfd, buf, strlen(buf));

        if (strncmp(buf, "get ", 4) == 0)
        { // The client wants to fetch a file
            char filename[SMALL_BUF];
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
