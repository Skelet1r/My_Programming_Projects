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
static const char* new_line = "\n";

void print_help()
{
	printf("PUT [KEY] [VALUE] - creates a new key-value pare\n");
	printf("GET [KEY] - show tables in current database\n");
	printf("DELETE TABLE [TABLE_NAME]\n");
	printf("[q] or [quit] or [exit] - exit\n");
}

void put(const char* key, const char* value) 
{   
    int fd, write_res;

    fd = open(table_name, O_CREAT | O_WRONLY | O_APPEND, file_perms);
    check_fd(fd, "open");

    write_res = write(fd, key, strlen(key));
    check_fd(write_res, "write");
	
	write_res = write(fd, separator, strlen(separator)); 
    check_fd(write_res, "write");

	write_res = write(fd, value, strlen(value));
    check_fd(write_res, "write");

	write_res = write(fd, new_line, strlen(new_line));
	check_fd(write_res, "write");
	close(fd);
}

void get(const char* key)
{
	char* buff;
	int fd;
	ssize_t bytes_read;

	fd = open(table_name, O_RDONLY);
	check_fd(fd, "open");

	buff = malloc(sizeof(char) * buff_len - 1);
	check_alloc(buff);
	
	bytes_read = read(fd, buff, buff_len - 1);
	while (bytes_read != EOF) {

	}
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
