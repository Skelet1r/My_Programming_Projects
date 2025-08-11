#include <netinet/in.h>
void error_check(const int res, const int sockfd);
struct sockaddr_in init_addr(const int port, const char* ip);
