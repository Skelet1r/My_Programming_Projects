#include <unistd.h>
#include <errno.h>
#include <signal.h>

static const char message[] = "Press Ctrl-C to quit\n";
static const char bye[] = "Goodbye!\n";
volatile static int n = 0;

static void handler(int s)
{
    int save_errno = errno;
    signal(SIGINT, handler);
    n++;
    errno = save_errno;
}

int main()
{
    write(1, message, sizeof(message) - 1);
    signal(SIGINT, handler); 
    while (1) {
        if (n) {
            write(1, bye, sizeof(bye) - 1);
            break;
        }
        sleep(1);
    }
    return 0;
}
