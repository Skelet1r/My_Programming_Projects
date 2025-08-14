#ifndef COMMANDS_PARSER_H
#define COMMANDS_PARSER_H

#include "word_item.h" 

void read_command(struct word_item** head, struct word_item** tail);
void parse_create_table(struct word_item** head);

#endif
