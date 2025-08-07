#include <stdio.h>
#include <string.h>

char* find_substring(char* str, char* substr);

int main(int argc, char* argv[])
{
    char* result;

    if (argc < 3) {
        printf("Usage: %s <string> <substring>\n", argv[0]);
        return 1;
    }

    result = find_substring(argv[1], argv[2]);

    if (result) {
        printf("Found: %s\n", result);
    } else {
        printf("Substring not found.\n");
    }

    return 0;
}

char* find_substring(char* str, char* substr)
{
    int i, j;

    if (!*substr) { 
        return str;
    }

    for (i = 0; str[i] != '\0'; i++) {
        j = 0;

        while (str[i + j] != '\0' && substr[j] != '\0' && str[i + j] == substr[j]) {
            j++;
        }

        if (substr[j] == '\0') {
            return &str[i]; 
        }
    }
    return NULL; 
}

