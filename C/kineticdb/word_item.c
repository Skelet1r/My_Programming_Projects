#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "includes/word_item.h"

struct word_item* add_word(struct word_item** head, struct word_item** tail, char* command)
{
    struct word_item* node_word;

    node_word = malloc(sizeof(struct word_item));
    if (!node_word) {
        perror("ERROR: An error while mallocing memory.\n");
		return NULL;
    }
    node_word->word = strdup(command);
    node_word->next = NULL;

    if (*head == NULL) {
        *head = node_word;
        *tail = node_word;
    } else {
        (*tail)->next = node_word;
        *tail = node_word;
    }
    return node_word;
}

int count_list_elements(struct word_item* head)
{
    int counter;
    struct word_item* curr;

    curr = head;
    counter = 0;

    while (curr != NULL) {
        counter++; 
        curr = curr->next;
    }
    return counter;
}

void print_list(struct word_item* head)
{
    struct word_item* curr; 
    curr = head;

    while (curr != NULL) {
        printf("[%s]\n", curr->word);
        curr = curr->next;
    }
}

void delete_list(struct word_item** head)
{
    struct word_item* curr;
    curr = *head;

    while (curr != NULL) {
        struct word_item* next;
        next = curr->next;
        free(curr->word);
        free(curr);
        curr = next;
    }
}
