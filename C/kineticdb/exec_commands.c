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
#include "includes/commands.h"

static void help()
{
	printf("CREATE TABLE [TABLE_NAME] [KEY] [VALUE] - creates a new key-value table\n");
	printf("SHOW TABLES - show tables in current database\n");
	printf("DELETE TABLE [TABLE_NAME]\n");
	printf("[q] or [quit] or [exit] - exit\n");
}

void exec_commands(struct word_item** head)
{
    const int list_elements = count_list_elements(*head);
    
    switch (list_elements) {
		case 1:
			quit(head);			
			break;
        case 2:
            parse_show_tables(head);
            break;
		case 3: 
			parse_delete_table(head);
			break;
        case 5: 
            parse_create_table(head);
            break;
		default:
            write(standart_output, unknown_command, strlen(unknown_command));
			break;
    }
}
