#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

enum { user_input_length = 256, time_limit = 15 };
static const char* question = "What is your name please?\n";
static const char* no_input = "Sorry, I'm terribly busy.\n";

static struct timeval init_timeout()
{
    struct timeval time;
    time.tv_sec = time_limit;
    time.tv_usec = 0;
    return time;
}

static void check_select_res(const int select_res)
{
    char user_input[user_input_length];
    if (select_res == -1) {
        perror("select");
        exit(1);
    }
    if (select_res == 0) {
        printf("%s", no_input);
    } else { 
        scanf("%s", user_input);
        printf("Nice to meet you, dear %s!\n", user_input);
    }
}

int main()
{
    fd_set timeoutfd;
    struct timeval timeout;
    int select_res;
   
    FD_ZERO(&timeoutfd);
    FD_SET(STDIN_FILENO, &timeoutfd);
   
    timeout = init_timeout(); 

    printf("%s", question);
    
    select_res = select(STDIN_FILENO + 1, &timeoutfd, NULL, NULL, &timeout);
    check_select_res(select_res);
    return 0;
}
