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

#include "utils.h"

enum { buff_len = 256, std_in = 0, std_out = 1, std_err = 2 };
static const char* black_hole = "/dev/null";
static const char* root_dir = "/";
static const char* program_name = "udp_server";
static const char* daemonization_success_message = "Server successfully started";
static const char* send_success_message = "Message successfully sended";
static const char* recv_success_message = "Message successfully recieved";
static const char* bind_success_message = "Socket successfully binded";

/* This function demonizes our UDP server */
static void daemonize()
{
    int pid;

    /* Close all standard file discriptors */
    close(std_in);
    close(std_out);
    close(std_err);   

    /* 
     * Open std again but all data and input will sending 
     * in `black hole` which is place where all data deleting 
     */
    open(black_hole, O_WRONLY); /* stdin */
    open(black_hole, O_WRONLY); /* stdout */ 
    open(black_hole, O_WRONLY); /* stderr */
    
    /* change dir to root-dir */
    chdir(root_dir); 
    
    /* create child process */
    pid = fork();
    if (pid > 0) {
        exit(0);
    }
    
    /* create new session for daemon */
    setsid();

    /*  
     * second process to guarantee that daemon 
     * won't become a leader of the process 
     */
    pid = fork();
    if (pid > 0) {
        exit(0);
    }

    syslog(LOG_INFO, "%s", daemonization_success_message);
}

static void send_message(const int sockfd, int sending_data, int* datagrams_count, struct sockaddr_in* client_addr)
{
    const int size_of_sending_data = sizeof(sending_data);
    int send_res;
    send_res = sendto(sockfd, &sending_data, size_of_sending_data, 0,
                            (struct sockaddr*)client_addr, sizeof(*client_addr));
    error_check(send_res, sockfd);
    *datagrams_count += 1;
    syslog(LOG_INFO, "%s", send_success_message);
}

static void recv_message(int sockfd, int* buff_size, struct sockaddr_in* client_addr)
{
    char buff[buff_len];
    int recvfrom_res;
    socklen_t addr_len;

    addr_len = sizeof(*client_addr); 
    recvfrom_res = recvfrom(sockfd, buff, sizeof(buff), 0, 
                (struct sockaddr*)client_addr, &addr_len);
    error_check(recvfrom_res, sockfd);
    *buff_size += recvfrom_res;
    syslog(LOG_INFO, "%s", recv_success_message);
}

static void bind_socket(int sockfd, struct sockaddr_in* client_addr)
{
    int bind_res;
    bind_res = bind(sockfd, (struct sockaddr*)client_addr, sizeof(*client_addr));
    error_check(bind_res, sockfd);
    syslog(LOG_INFO, "%s", bind_success_message);
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        perror("Usage: ./client [PORT] [IP]\n");
        exit(1);
    }
    daemonize();
    openlog(program_name, LOG_PID | LOG_CONS, LOG_USER);
    const int port = atoi(argv[1]);
    const char* ip = argv[2];
    int datagrams_count, buff_size;
    datagrams_count = 0;

    struct sockaddr_in client_addr;
    int sockfd;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    client_addr = init_addr(port, ip);

    bind_socket(sockfd, &client_addr);
   
    while (1) {
        recv_message(sockfd, &buff_size, &client_addr);

        send_message(sockfd, datagrams_count, &datagrams_count, &client_addr);
        send_message(sockfd, buff_size, &datagrams_count, &client_addr);
    }
    close(sockfd);
    closelog();
    return 0;
}
