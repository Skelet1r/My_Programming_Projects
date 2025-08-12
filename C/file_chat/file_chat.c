#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

enum { buff_len = 1024, pipe_elements = 2, file_perms = 0644 };
static const char* usage = "Usage: ./file_chat [FILE1] [FILE2]\n";

static void check_error(const int fd)
{
    if (fd == -1) {
        perror("open");
        exit(1);
    }
}

static void print_file_content(const char* file_name) 
{
    size_t bytesRead;
    char buffer[buff_len];
    int fd;

    fd = open(file_name, O_RDONLY);
    check_error(fd);

    while ((bytesRead = read(fd, buffer, buff_len)) > 0) {
        for (ssize_t i = 0; i < bytesRead; i++) {
            printf("%c", buffer[i]);
        }
    }
    printf("\n");
}

static void write_file(const char* file_name, const char* user_input)
{
    int fd, write_res, is_EOF; 

    fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, file_perms); 
    check_error(fd);

    write_res = write(fd, user_input, strlen(user_input));
    if (write_res == -1) {
        perror("write");
        close(fd);
        exit(1);
    }
    close(fd);
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        perror(usage);
        exit(2);
    }
    const char* finput = argv[1];
    const char* foutput = argv[2];

    while (1) {
        ssize_t recved_data_from_pipe;
        int pipe_res;
        int p[pipe_elements];
        char user_input[buff_len]; 
        char buff_for_data_from_pipe[buff_len];

        pipe_res = pipe(p);
        if (pipe_res == -1) {
            perror("pipe");
            exit(1);
        }
        
        scanf("%s", user_input);
        write(p[1], user_input, strlen(user_input));
        close(p[1]);

        recved_data_from_pipe = read(p[0], buff_for_data_from_pipe, sizeof(buff_for_data_from_pipe) - 1);
        if (recved_data_from_pipe >= 0) {
            buff_for_data_from_pipe[recved_data_from_pipe] = '\0';
            write_file(finput, buff_for_data_from_pipe);
        }
        print_file_content(foutput);
        close(p[0]);
    }
}
