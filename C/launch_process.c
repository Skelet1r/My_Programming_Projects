#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    char* cmdline[argc];
    int pid;

    if (argc < 2) {
        perror("Usage: ./launch_process <command> <args>\n");
        return 1;
    }
    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        for (int i = 1; i < argc; i++) {
            cmdline[i - 1] = argv[i];
        }
        cmdline[argc - 1] = NULL;
        execvp(cmdline[0], cmdline);
    }
    wait(NULL);
    return 0;
}
