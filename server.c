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
#include <signal.h>

#define BUF_SIZE 256

void check_error(int status)
{
    if (status == -1)
    {
        printf("Error (%d): %s\n", errno, strerror(errno));
    }
}
void setup_new_handshake()
{
    printf("Awaiting Client Connection...\n");
    // wkp stands for the server's well known pipe
    mkfifo("./wkp", 0666);
    int fd;
    fd = open("./wkp", O_RDONLY);
    char secret_path[BUF_SIZE + 2];
    int status;
    status = read(fd, secret_path, BUF_SIZE + 2);
    int secret_pipe = open(secret_path, O_WRONLY);
    remove("./wkp");
    check_error(status);
    // Client sends to server first
    printf("Handshake Commencing with client (Path=%s)\n\n", secret_path);
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
}
static void sighandler(int signo)
{
    if (signo == SIGPIPE)
    {
        printf("Client disconnected. Setting up new handshake for next client...\n");
        setup_new_handshake();
    }

    if (signo == SIGINT)
    {
        remove("./client_pipe");
        remove("./server_pipe");
        remove("./wkp");
        exit(0);
    }
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
    int processed;
    int fd1, fd2;
    fd1 = open("./client_pipe", O_RDONLY);
    fd2 = open("./server_pipe", O_WRONLY);

    char client_input[BUF_SIZE];
    while (1)
    {
        read(fd1, client_input, BUF_SIZE);
        printf("getting number of vowels for \"%s\"...\n", client_input);
        processed = determine_number_of_vowels(client_input);
        write(fd2, &processed, sizeof(processed));
    }
    close(fd1);
    close(fd2);
}

int main()
{
    mkfifo("./client_pipe", 0666);
    mkfifo("./server_pipe", 0666);
    signal(SIGPIPE, sighandler);
    signal(SIGINT, sighandler);
    setup_new_handshake();
    int processed;
    int fd1, fd2;
    serve();
    remove("./client_pipe");
    remove("./server_pipe");
    return 0;
}
