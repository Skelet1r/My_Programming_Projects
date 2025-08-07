#include <stdio.h>
#include <stdlib.h>

struct item {
    int x;
    struct item *next;
};

struct item* build_list_from_array(int* arr, int length_of_arr);
int* build_arr(struct item* head);
int count_elements_in_list(struct item* head);

int main()
{
    struct item* first;
    int arr[5] = { 1, 2, 3, 4, 5 }; 
    int size_of_arr;
   
    size_of_arr = 5;
    first = build_list_from_array(arr, size_of_arr);
    
    int* array = build_arr(first);

    for (int i = 0; i < size_of_arr; i++) { 
        printf("%d\n", array[i]);
    }
    return 0;
}

struct item* build_list_from_array(int* arr, int length_of_arr)
{
    struct item* head = NULL;
    struct item* current = NULL;

    for (int i = 0; i < length_of_arr; i++) { 
        struct item* node;
        node = malloc(sizeof(struct item));

        node->x = arr[i];
        node->next = NULL;

        if (head == NULL) {
            head = node;
            current = node;
        } else {
            current->next = node;
            current = node;
        }
    }
    return head;
}

int count_elements_in_list(struct item* head)
{
    int counter;
    counter = 0;

    while (head != NULL) {
        counter++;             
        head = head->next;
    }
    return counter; 
}

int* build_arr(struct item* head)
{
    int size_of_array, i;
    i = 0;
    size_of_array = count_elements_in_list(head);

    int* arr = malloc(size_of_array * sizeof(int));

    while (head != NULL) {
        arr[i] = head->x;
        i++;         
        head = head->next;
    }
    return arr;
}

