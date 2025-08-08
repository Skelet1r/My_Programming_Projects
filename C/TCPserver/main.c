#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/syslog.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <stddef.h>
#include <string.h>
#include <time.h>

enum { qlen = 16, buff_len = 256, reuse_addr_on = 1 };
static const char* usage = "Usage: ./tcp_server [PORT]";
static const char* new_line = "\n";

static const char* get_time()
{
    struct tm * timeinfo;
    time_t rawtime;
    time(&rawtime);
    timeinfo = localtime(&rawtime);  
    const char* time = asctime(timeinfo);
    return time;
}

static void error_check(const int res, const int sockfd)
{
    if (res == -1) {
        perror("sockfd");
        close(sockfd);
        exit(1);
    }
}

static struct sockaddr_in init_addr(const int port)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    return addr;
}

static void send_to_client(const int clientfd, const int serverfd, const void* buff)
{
    int send_res;
    send_res = send(clientfd, buff, strlen(buff), 0);
    if (send_res == -1) {
        perror("send");
        close(serverfd);
        close(clientfd);
        exit(1);
    }
}

static void bind_sock(const int serverfd, struct sockaddr_in server_addr)
{
    int server_bind_res;
    server_bind_res = bind(serverfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    error_check(server_bind_res, serverfd);
}

static void listen_server(const int serverfd)
{
    int server_listen_res;
    server_listen_res = listen(serverfd, qlen);
    error_check(server_listen_res, serverfd);
}

static int accept_connection(const int serverfd, struct sockaddr_in* client_addr)
{
    int clientfd; 
    socklen_t client_addr_size;

    client_addr_size = sizeof(*client_addr);
    clientfd = accept(serverfd, (struct sockaddr*)client_addr, &client_addr_size);
    error_check(clientfd, serverfd);
    return clientfd;
}

static void recv_message(const int clientfd)
{
    char buff[buff_len];
    int recv_res;
    recv_res = recv(clientfd, buff, sizeof(buff), 0);
    error_check(recv_res, clientfd);
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        perror(usage);
        exit(1);
    }
    const int port = atoi(argv[1]);
    const char* str_port = argv[1];

    while (1) {
        const char* time = get_time();
        struct sockaddr_in server_addr;
        struct sockaddr_in client_addr;
        int serverfd, clientfd, opt;
        char client_ip[buff_len];

        opt = reuse_addr_on;
        serverfd = socket(AF_INET, SOCK_STREAM, 0);
        setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        server_addr = init_addr(port);

        bind_sock(serverfd, server_addr);
        listen_server(serverfd);
        
        clientfd = accept_connection(serverfd, &client_addr);
       
        recv_message(clientfd);

        /* get client's ip address */
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
       
        send_to_client(clientfd, serverfd, client_ip);
        send_to_client(clientfd, serverfd, new_line);
        send_to_client(clientfd, serverfd, str_port);
        send_to_client(clientfd, serverfd, new_line);
        send_to_client(clientfd, serverfd, time);

        printf("%s\n", client_ip);
        close(serverfd);
        close(clientfd);
    }
}
