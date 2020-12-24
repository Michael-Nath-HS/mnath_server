#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

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

int main()
{
    int MAX_SIZE = 256;
    char input[MAX_SIZE];
    int processed;
    mkfifo("./mario", 0666);
    mkfifo("./luigi", 0666);
    while (1)
    {
        int fd;
        fd = open("./mario", O_RDONLY);
        read(fd, input, MAX_SIZE);
        close(fd);
        processed = determine_number_of_vowels(input);
        fd = open("./luigi", O_WRONLY);
        write(fd, &processed, sizeof(processed));
        close(fd);
    }
    return 0;
}