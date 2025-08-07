#include <stdio.h>
#include <string.h>

int count_spaces(char *str);

int main()
{
    int count;
    char str[256];

    printf("Enter string: ");
    fgets(str, sizeof(str), stdin); 

    count = count_spaces(str);
    printf("Count of spaces: %d\n", count);
    
    return 0;
}

int count_spaces(char *str)
{
    int counter;
    counter = 0;

    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == ' ') {
            counter++;
        }
    }
    return counter;
}

