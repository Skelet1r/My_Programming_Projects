#include <stdio.h>

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            continue;
        } else {
            printf("%s\n", argv[i]);
        }
    }
    return 0;
}
