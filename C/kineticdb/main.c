#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "includes/word_item.h"
#include "includes/commands_parser.h"
#include "includes/exec_commands.h"

static const char* path_to_kineticdb = "/var/lib/kineticdb";

int main()
{
    chdir(path_to_kineticdb);
    while (1) {
        printf("> ");

        struct word_item* head;
        struct word_item* tail;

        head = NULL;
        tail = NULL;
    
        read_command(&head, &tail);
        exec_commands(&head);
        delete_list(&head);
    }     
    return 0;
}
