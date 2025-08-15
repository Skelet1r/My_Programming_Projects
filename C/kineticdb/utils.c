#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "includes/utils.h"

static const char* error_message = "ERROR: allocation error\n";

void check_alloc(const void* word) 
{
    if (!word) {
        printf("%s", error_message);
        exit(1);
    }
}

void check_fd(const int fd, const char* action)
{
    if (fd == -1) {
        perror(action);
        close(fd);
        exit(1);
    }
}

void check_pid(const int pid, const char* action) 
{
    if (pid == -1) {
        perror(action);
        exit(1);
    }
}
