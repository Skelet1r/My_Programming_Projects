#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

void read_file(const char* word, const char* file)
{
    int pid;

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        execlp("grep", "grep", word, file, NULL); 
        perror("grep");
        exit(1);
    }
    wait(NULL);
}

int main(int argc, char** argv)
{
    const char* word = argv[1];
    const char* file = argv[2];
    read_file(word, file);
    return 0;
}
