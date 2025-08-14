#ifndef EXEC_COMMANDS_H
#define EXEC_COMMANDS_H

#include "word_item.h"

void exec_commands(struct word_item** head);
void create_table(const char* table_name, const char* key, const char* value);

#endif
