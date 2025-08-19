#ifndef COMMANDS_H
#define COMMANDS_H

#include "word_item.h"

void put(const char* key, const char* value);
void show_tables();
void quit(struct word_item** head);
void delete_table(const char* table_name);

#endif
