#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "includes/word_item.h"
#include "includes/commands_parser.h"
#include "includes/exec_commands.h"

int main()
{
    while (1) {
        printf("> ");

        struct word_item* head;
        struct word_item* tail;

        head = NULL;
        tail = NULL;
    
        read_command(&head, &tail);
//        print_list(head);
        exec_commands(&head);
        delete_list(&head);
    }     
    return 0;
}
