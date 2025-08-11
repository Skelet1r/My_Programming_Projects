#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <stddef.h>
#include <sys/time.h>

#include "utils.h"

enum { seconds_limit = 15, micro_seconds = 0 };

int main(int argc, char** argv)
{
    if (argc != 4) {
        perror("Usage: ./client [PORT] [IP] [MESSAGE]\n");
        exit(1);
    }
    const int port = atoi(argv[1]);
    const char* ip = argv[2];
    const char* message = argv[3];
    const size_t message_size = strlen(message);
    
    struct timeval timeout;
    struct sockaddr_in server_addr;
    socklen_t addr_len;
    int sockfd, sendto_res, recv_dg_count_res, recv_buff_size_res, dg_count, buff_size;

    timeout.tv_sec = seconds_limit;
    timeout.tv_usec = micro_seconds;

    server_addr = init_addr(port, ip);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    sendto_res = sendto(sockfd, message, message_size, 0, 
                (struct sockaddr*)&server_addr, sizeof(server_addr));
    error_check(sendto_res, sockfd);
    
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    addr_len = sizeof(server_addr);
    recv_dg_count_res = recvfrom(sockfd, &dg_count, sizeof(dg_count), 0,
                                    (struct sockaddr*)&server_addr, &addr_len);
    error_check(recv_dg_count_res, sockfd); 

    recv_buff_size_res = recvfrom(sockfd, &buff_size, sizeof(buff_size), 0,
                                    (struct sockaddr*)&server_addr, &addr_len);
    error_check(recv_buff_size_res, sockfd);
    printf("Datagrams count: %d\nDatagrams total size: %d\n", dg_count, buff_size);
    close(sockfd);
    return 0;
}
