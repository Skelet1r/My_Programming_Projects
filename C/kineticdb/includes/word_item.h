struct word_item {
    char *word;
    struct word_item *next;
};

struct word_item* add_word(struct word_item** head, struct word_item** tail, char* command);
void print_list(struct word_item* head);
void delete_list(struct word_item** head);
int count_list_elements(struct word_item* head);
