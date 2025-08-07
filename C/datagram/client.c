#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

static const int server_response_size = 1024;

static void connect_to_server(int sockfd, struct sockaddr_in server_address)
{
    int connection_status;
    connection_status = connect(sockfd, (struct sockaddr *) &server_address, sizeof(server_address));
    if (connection_status == -1) {
        perror("connect");
        exit(1);
    }
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        perror("Usage: ./client [PORT]\n");
        exit(1);
    }
    const int port = atoi(argv[1]);

    char server_response[server_response_size];
    int sockfd, connection_status;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    connect_to_server(sockfd, server_address);

    recv(sockfd, &server_response, sizeof(server_response), 0);
    printf("%s\n", server_response);
    close(sockfd);
    return 0;
}
