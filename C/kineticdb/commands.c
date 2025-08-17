#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

#include "includes/commands.h"
#include "includes/word_item.h"
#include "includes/utils.h"

static const char* separator = ":";

void create_table(
    const char* table_name, 
    const char* key, 
    const char* value
) {   
    int fd, write_res;

    fd = open(table_name, O_CREAT | O_WRONLY | O_APPEND, file_perms);
    check_fd(fd, "open");

    write_res = write(fd, key, strlen(key));
    check_fd(write_res, "write");
	
	write_res = write(fd, separator, strlen(separator)); 
    check_fd(write_res, "write");

	write_res = write(fd, value, strlen(value));
    check_fd(write_res, "write");
	close(fd);
}

void show_tables()
{
    char* command[] = { "ls", NULL };
    int pid;

    pid = fork();
    check_res(pid, "fork");

    if (pid == 0) {
        execvp("ls", command);
        perror("ls");
        exit(1);
    }
    wait(NULL);
}

void delete_table(const char* table_name)
{
	int unlink_res;
	unlink_res = unlink(table_name);
	check_res(unlink_res, "unlink");
}

void quit(struct word_item** head)
{
	struct word_item* curr;
	curr = *head;
	if (0 == strcmp(curr->word, "q")    ||
		0 == strcmp(curr->word, "quit") ||
		0 == strcmp(curr->word, "exit")) 
		exit(0);	
}

