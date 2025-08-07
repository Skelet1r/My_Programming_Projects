#include <stdio.h>
#include <stdlib.h>

struct item {
    int x;
    struct item *next;
};

struct item* add_element(struct item** head, struct item** tail, int x);
static void print_list(struct item* head);
static void delete_list(struct item** head);
void print_meet_three_times(struct item* head);

int main()
{
    int arr[256];
    int num;
    struct item* head;
    struct item* tail; 
    
    head = NULL;
    tail = NULL;

    while (scanf("%d", &num) != EOF) {
        add_element(&head, &tail, num); 
    }

    printf("\nFigures that meet three times: \n");
    print_meet_three_times(head);
    delete_list(&head);

    return 0;
}

struct item* add_element(struct item** head, struct item** tail, int x)
{
    struct item* node; 

    node = malloc(sizeof(struct item));
    node->x = x;
    node->next = NULL;

    if (*head == NULL) {
        *head = node;
        *tail = node;
    } else {
        (*tail)->next = node;
        *tail = node;
    }
    return node;
}

static void print_list(struct item* head)
{
    struct item* curr;
    curr = head;

    while (curr != NULL) {
        printf("%d\n", curr->x);
        curr = curr->next;
    }
}

static void delete_list(struct item** head)
{
    struct item* curr;
    curr = *head;

    while (curr != NULL) {
        struct item* next;
        next = curr->next;
        free(curr);
        curr = next;
    }
}

void print_meet_three_times(struct item* head)
{
    struct item* curr1;
    curr1 = head;

    while (curr1 != NULL) {
        int meets;
        struct item* curr2;

        meets = 0;
        curr2 = head;

        while (curr2 != NULL) {
            if (curr1->x == curr2->x) {
                meets++;
            }
            curr2 = curr2->next;
        }

        if (meets == 3) {
            int meeted;
            struct item* prev;

            prev = head;
            meeted = 0;

            while (prev != curr1) { 
                if (prev->x == curr1->x) {
                    meeted++;
                    break;
                }
                prev = prev->next;
            }

            if (!meeted) {
                printf("%d\n", curr1->x);
                meets = 0;
            }
        }
        curr1 = curr1->next; 
    }
}
