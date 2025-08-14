#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "includes/commands_parser.h"
#include "includes/exec_commands.h"
#include "includes/word_item.h"
#include "includes/utils.h"

enum { file_perms = 0644 };

static void help()
{
	printf("CREATE TABLE [TABLE_NAME] [KEY] [VALUE]");
}

void 
create_table(const char* table_name, const char* key, const char* value)
{   
    int fd, write_res;

    fd = open(table_name, O_CREAT | O_WRONLY, file_perms);
    check_fd(fd, "open");

    write_res = write(fd, key, strlen(key));
    check_fd(fd, "write");
    
    write_res = write(fd, value, strlen(value));
    check_fd(fd, "write");
}

void exec_commands(struct word_item** head)
{
    const int list_elements = count_list_elements(*head);
    
    switch (list_elements) {
        case 5: 
            parse_create_table(head);
            break;
    }
}
