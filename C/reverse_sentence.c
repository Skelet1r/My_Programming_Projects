#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reverse_sentence(int size, char* str);

int main()
{
    char* str;
    int size, len;
    
    while(scanf("%d", &size) != EOF) { 
        getchar();
        str = malloc(size + 1);

        if (str == NULL) {
            printf("ERROR: An error while malloc memory\n");
            return 1;
        }

        fgets(str, size + 1, stdin);
        reverse_sentence(size, str);
    }
}

void reverse_sentence(int size, char* str)
{
    for (int i = size; i >= 0; i--) {
        printf("%c", str[i]);
    }
    printf("\n");

    free(str);
}
