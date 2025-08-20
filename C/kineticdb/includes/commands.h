#ifndef COMMANDS_H
#define COMMANDS_H

#include "word_item.h"

void put(const char* key, const char* value);
void get(const char* key);
void quit(struct word_item** head);
void delete_table(const char* table_name);
void print_help();

#endif
