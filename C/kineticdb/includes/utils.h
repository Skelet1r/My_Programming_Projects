enum {
    file_perms = 0644,
    dir_perms = 0755,
    buffer_len = 1024, 
    standart_output = 1,
};

static const char* unknown_command = "Unknown command\n";

void check_alloc(const void* word);
void check_fd(const int fd, const char* action);
void check_pid(const int pid, const char* action);
