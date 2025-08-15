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

static const char* path_to_kineticdb = "kineticdb";
static const char* error_message = "An error while checking directory\n";

static int is_kineticdb_exists()
{
    DIR *dir;
    dir = opendir(path_to_kineticdb);

    if (dir) {
        return 1;
    } else if (ENOENT == errno) {
        return 0;
    } 
    return -1;
}

int main()
{
    int is_kineticdb_exists_res;
    is_kineticdb_exists_res = is_kineticdb_exists(); 

    if (is_kineticdb_exists_res == 0) {
        mkdir(path_to_kineticdb, 0755);
    } else if (is_kineticdb_exists_res == -1) {
        perror(error_message);
        exit(1);
    }
    
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
