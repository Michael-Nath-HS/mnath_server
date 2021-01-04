#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/errno.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#define BUF_SIZE 256
static void sighandler(int signo)
{

    if (signo == SIGINT)
    {
        remove("./server_pipe");
        remove("./client_pipe");
        exit(0);
    }
}

void check_error(int status)
{
    if (status == -1)
    {
        printf("Error (%d): %s\n", errno, strerror(errno));
    }
}
void send_handshake()
{
    char secret_path[BUF_SIZE];
    sprintf(secret_path, "./%d", getpid());
    mkfifo(secret_path, 0666);
    char wellknown[] = "./wkp";
    int status;
    int wkp, secret_pipe;
    wkp = open(wellknown, O_WRONLY);
    status = write(wkp, secret_path, 4 * strlen(secret_path));
    secret_pipe = open(secret_path, O_RDONLY);
    // sending initial connection request to server
    check_error(status);
    // receiving acknowledgement from server
    char msg[BUF_SIZE];
    status = read(secret_pipe, msg, BUF_SIZE);
    check_error(status);
    // letting server know we got acknowledgement
    char confirm_msg[] = "Yeah got the acknowledgement!\n";
    status = write(wkp, confirm_msg, sizeof(confirm_msg));
    check_error(status);
    close(wkp);
    close(secret_pipe);
    remove(secret_path);
}

int main()
{
    printf("Give a word(s), and it will tell you how many vowels there are...\n");
    signal(SIGINT, sighandler);
    mkfifo("./client_pipe", 0666);
    mkfifo("./server_pipe", 0666);
    char input[BUF_SIZE];
    int response;
    send_handshake();
    int fd1, fd2;
    // open the fifo "client_pipe" in order to send input
    fd1 = open("./client_pipe", O_WRONLY);
    // open the fifo "server_pipe" in order to receive response
    fd2 = open("./server_pipe", O_RDONLY);
    while (1)
    {
        // store the input in the input character array
        fgets(input, BUF_SIZE, stdin);
        write(fd1, input, 4 * strlen(input));
        read(fd2, &response, BUF_SIZE);
        printf("%d\n", response);
    }
}