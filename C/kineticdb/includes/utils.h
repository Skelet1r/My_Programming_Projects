#ifndef UTILS_H
#define UTILS_H

enum {
    file_perms = 0644,
    dir_perms = 0755,
    buffer_len = 1024, 
    standart_output = 1,
};

extern const char* unknown_command;
extern const char* table_name;

void check_alloc(const void* word);
void check_fd(const int fd, const char* action);
void check_res(const int res, const char* action);

#endif
