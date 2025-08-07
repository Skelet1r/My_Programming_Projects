#include <stdio.h>

int main(int argc, char* argv[])
{
    FILE* file;
    int ch;

    file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("ERROR: An error while opening a file.\n");
        return 1;
    }

    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }
    return 0;
}
