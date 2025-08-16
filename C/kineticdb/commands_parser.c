#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "includes/utils.h"
#include "includes/word_item.h"
#include "includes/exec_commands.h"
#include "includes/commands.h"

static const char* quotes_error_message = "ERROR: unmatched quotes\n";

static char* parse_quotes(char type_of_quotes)
{    
    char* word;
    int j;
    char c;
    
    word = malloc(buffer_len);
    check_alloc(word);
    j = 0;
    while ((c = getchar()) != type_of_quotes) {
        if (c == '\n') {
            perror(quotes_error_message);
            free(word);
            return NULL;
        }
        word[j] = c;
        j++;
    }
    word[j] = '\0';
    return word;
}

static 
void 
reset_word(char** command, int* i, struct word_item** head, struct word_item** tail)
{    
    (*command)[(*i)] = '\0';
    add_word(head, tail, *command);
    free(*command);
    *command = malloc(buffer_len);
    check_alloc(*command);
    *i = 0;
}

void parse_create_table(struct word_item** head)
{
    struct word_item* curr;
    curr = *head;

    for (int i = 0; i < 2; i++) {
        if (0 == strcmp(curr->word, "CREATE") && i == 0) { 
            curr = curr->next;
        } else if (0 == strcmp(curr->word, "TABLE") && i == 1) {
            curr = curr->next;
        }  else {
            write(standart_output, unknown_command, strlen(unknown_command));
            return;
        }
    }
    const char* table_name = curr->next->word;
    const char* key = curr->next->next->word;
    const char* value = curr->next->next->next->word;
    create_table(table_name, key, value);
}            

void parse_show_tables(struct word_item** head)
{
    struct word_item* curr;
    curr = *head;
    
    if (0 == strcmp(curr->word, "SHOW") && 
        0 == strcmp(curr->next->word, "TABLES")) {
        show_tables();   
    }  
}

void read_command(struct word_item** head, struct word_item** tail)
{
    char* command;    
    char c;
    int i;
   
    i = 0;
    command = malloc(buffer_len);
    check_alloc(command); 

    while ((c = getchar()) != '\n') {
        if (c == ' ') {
            reset_word(&command, &i, head, tail);
        } else if (c =='"' || c == '\'') {
            char* tmp;
            tmp = parse_quotes(c);        
            if (tmp != NULL) {
                add_word(head, tail, tmp);
                free(tmp);
            } else {
                free(command);
                return;
            }
        } else { 
            command[i++] = c;
        }
    }   
    if (i > 0) {
        reset_word(&command, &i, head, tail);
    }
}
