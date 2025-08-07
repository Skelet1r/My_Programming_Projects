#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define ERROR_CODE 1
#define SIZE_OF_CHAR sizeof(char)
#define BUFFER_SIZE 1024
#define FILE_NAME "test.txt"
#define PERMS 0600

static void error(const int fd);
static void print_file_content(const char* file_name);
static int incriment(int number);

int main(int argc, char* argv[])
{
    if (argc == 1) {
        while (1) {
            char c;
            while ((c = getchar()) != '\n') {
                putchar(c); 
            } 
        }
    } else {
        int count_of_iterations;
        count_of_iterations = incriment(argc);
        
        for (int i = 1; i < count_of_iterations; i++) {
            print_file_content(argv[i]);
        }
    }
    return 0;
}

static void error(const int fd)
{
    if (fd == -1) {
        printf("ERROR: An error while working with file.\n");
        exit(ERROR_CODE);
    }
}

static void print_file_content(const char* file_name) 
{
    size_t bytesRead;
    char buffer[BUFFER_SIZE];
    int fd;

    fd = open(file_name, O_RDONLY);
    error(fd);

    while ((bytesRead = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (ssize_t i = 0; i < bytesRead; i++) {
            printf("%c", buffer[i]);
        }
    }
}

static int incriment(int number)
{
    return number++;
}
