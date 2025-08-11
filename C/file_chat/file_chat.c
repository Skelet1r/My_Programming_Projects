#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <string.h>

enum { buffer_size = 1024, file_perms = 0644 };
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
    char buffer[buffer_size];
    int fd;

    fd = open(file_name, O_RDONLY);
    check_error(fd);

    while ((bytesRead = read(fd, buffer, buffer_size)) > 0) {
        for (ssize_t i = 0; i < bytesRead; i++) {
            printf("%c", buffer[i]);
        }
    }
    printf("\n");
}

static void write_file(const char* file_name)
{
    int fd, write_res, is_EOF; 
    char user_input[buffer_size];


    is_EOF = scanf("%s", user_input);
    if (is_EOF == EOF) 
        exit(0);

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
        int select_res, max_fd;
        char c2;
        fd_set foutputfd;
     
        print_file_content(finput);
        
        FD_ZERO(&foutputfd);
        FD_SET(0, &foutputfd);

        max_fd = 0;
        select_res = select(max_fd + 1, &foutputfd, NULL, NULL, NULL);
        if (select_res == -1) {
            perror("select");
            exit(1);
        }

        if (select_res == 0) 
            printf("Nothing happened\n");
        else 
            write_file(foutput);
    }
    return 0;
}
