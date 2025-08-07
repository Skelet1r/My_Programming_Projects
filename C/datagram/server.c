#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

enum { server_message_length = 42, max_queue_count = 16 };

static struct sockaddr_in init_sockaddr_in(const int ip_address, const int port)
{
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = ip_address;
    return server_address;
}

static void bind_to_server(int server_sockfd, struct sockaddr_in server_address)
{
    int bind_result;
    bind_result = bind(server_sockfd, (struct sockaddr*) &server_address, sizeof(server_address));
    if (bind_result == -1) {
        perror("bind");
        exit(1);
    }
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        perror("Usage: ./server [IP_ADDR] [PORT] [SERVER_MESSAGE]\n");
        exit(1);
    }

    const int ip_address = atoi(argv[1]);
    const int port = atoi(argv[2]);
    const char* server_message = argv[3];

    int server_sockfd, client_sockfd;
    struct sockaddr_in server_address;

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_address = init_sockaddr_in(ip_address, port);

    bind_to_server(server_sockfd, server_address);

    listen(server_sockfd, max_queue_count); 

    client_sockfd = accept(server_sockfd, NULL, NULL);

    send(client_sockfd, server_message, sizeof(server_message), 0);
    close(client_sockfd);
    return 0;
}
