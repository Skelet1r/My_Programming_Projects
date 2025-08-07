#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

static int is_file(const char* file_name);
static void search_file(const char* file_path);

int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("Usage: ./where [FILE]\n");
        exit(1);
    }
    
    const char* file_path = argv[1];
    search_file(file_path);
    return 0;
}

static int is_file(const char* file_name)
{
    struct stat path;
    int stat_res;

    stat_res = stat(file_name, &path);
    if (stat_res == S_ISDIR(path.st_mode)) {
        return 1;
    }
    return 0;
}

static void search_file(const char* file_path)
{
    const char* curr_dir = ".";
    DIR *dir;
    struct dirent *dent;
    int dirs, is_file_res;

    dir = opendir(curr_dir);
    if (!dir) {
        perror(curr_dir);
        exit(1);
    }

    while ((dent = readdir(dir)) != NULL) {
        const char* curr_d_name = dent->d_name;

        if (0 == strcmp(curr_d_name, ".") ||
            0 == strcmp(curr_d_name, "..")) {
            continue;
        }

        if (0 == strcmp(curr_d_name, file_path)) {
            const char* result = realpath(file_path, NULL);
            printf("%s\n", result);
            exit(0);
        }

        is_file_res = is_file(curr_d_name);
        if (!is_file_res) {
            chdir(curr_d_name);
//            search_file(file_path);
        }
    }
    closedir(dir);
}
