#include <stdio.h>

int main(int argc, char* argv[])
{
    char buffer[1024];
    FILE* file;
    
    file = fopen(argv[1], "w"); 
    if (file == NULL) {
        printf("ERROR: An error while opening file.\n");
        return 1;
    }

    while(fgets(buffer, sizeof(buffer), stdin)) {
        fputs(buffer, file);
    }

    pclose(file);
    return 0;
}
