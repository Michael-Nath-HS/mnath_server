#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <string.h>
#include <errno.h>
#define BUF_SIZE 256

void check_error(int status)
{
    if (status == -1)
    {
        printf("Error (%d): %s\n", errno, strerror(errno));
    }
}
void setup_new_handshake(int fd)
{
    int client_pid;
    int status;
    int secret_pipe = open("./secret", O_WRONLY);
    status = read(fd, &client_pid, sizeof(int));
    check_error(status);
    // Client sends to server first
    printf("Handshake Commencing with client (PID=%d)\n\n", client_pid);
    // Server verifies that client can send
    printf("Sending acknowledgement message to client\n\n");
    char message[] = "You've been acknowledged by server";
    status = write(secret_pipe, message, sizeof(message));
    check_error(status);
    char final_confirmation[BUF_SIZE];
    // Server verifies that client recieved acknowledgement
    status = read(fd, final_confirmation, BUF_SIZE);
    check_error(status);
    printf("Handshake established! Received final confirmation message: %s\n\n", final_confirmation);
    close(fd);
    remove("./wkp");
}

int determine_number_of_vowels(char *input)
{
    char *p = input;
    char lowercase;
    int num_lowercases = 0;
    while (*p != '\0')
    {
        lowercase = tolower(*p);
        if (lowercase == 'a' || lowercase == 'e' || lowercase == 'i' || lowercase == 'o' || lowercase == 'u')
        {
            num_lowercases++;
        }
        p++;
    }
    return num_lowercases;
};

void serve()
{
    mkfifo("./client_pipe", 0666);
    mkfifo("./server_pipe", 0666);
    int processed;
    int fd1, fd2;
    fd1 = open("./client_pipe", O_RDONLY);
    fd2 = open("./server_pipe", O_WRONLY);
    char client_input[BUF_SIZE];
    while (1)
    {
        read(fd1, client_input, BUF_SIZE);
        processed = determine_number_of_vowels(client_input);
        write(fd2, &processed, sizeof(processed));
    }
    close(fd1);
    close(fd2);
}

int main()
{
    // wkp stands for the server's well known pipe
    mkfifo("./wkp", 0666);
    int fd;
    fd = open("./wkp", O_RDONLY);
    setup_new_handshake(fd);
    int processed;
    int fd1, fd2;
    fd1 = open("./client_pipe", O_RDONLY);
    fd2 = open("./server_pipe", O_WRONLY);
    char client_input[BUF_SIZE];
    while (1)
    {
        read(fd1, client_input, BUF_SIZE);
        processed = determine_number_of_vowels(client_input);
        write(fd2, &processed, sizeof(processed));
    }
    close(fd1);
    close(fd2);
    remove("./client_pipe");
    remove("./server_pipe");
    return 0;
}