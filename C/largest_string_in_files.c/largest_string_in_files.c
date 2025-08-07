#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void print_largest_string_in_file(int i, FILE *file);

int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++) {
        FILE* file;

        file = fopen(argv[i], "r");
        if (file == NULL) {
            printf("ERROR: An error while opening a file.\n");
            return 1;
        }
        printf("%s:", argv[i]);
        print_largest_string_in_file(i, file);
    }
    return 0;
}

void print_largest_string_in_file(int i, FILE* file)
{
    int largest_string_length;
    char line[1024];
    char largest_string[1024];

    largest_string_length = 0;

    while (fgets(line, sizeof(line), file)) { 
        if (strlen(line) > largest_string_length) {
            largest_string_length = strlen(line);
            strcpy(largest_string, line);
        }
    }
    printf("%s", largest_string);
    fclose(file);
}
