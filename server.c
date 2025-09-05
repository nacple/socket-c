#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

struct sockaddr_in * createAddr(char *ip, int port);

struct AcceptedConnection {
    int acceptedSocketfd;
    struct sockaddr_in addr;
    int error;
    char status;
};

struct AcceptedConnection * acceptConnection(int server_socketfd);

int main() {
    int server_socketfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in * server_addr = createAddr("127.0.0.1", 9888);

    int result = bind(server_socketfd, server_addr, sizeof(*server_addr));
    if (result < 0) {
        printf("Error: bind()\n");
        return 1;  
    } printf("Server started!\n");

    result = listen(server_socketfd, 10);
    if (result < 0) {
        printf("Error: listen()\n");
        return 1;
    } printf("Listening...\n");

    struct AcceptedConnection *client_socket = acceptConnection(server_socketfd);

    char buffer[1024];
    while(1) {
        ssize_t result_recv = recv(client_socketfd, buffer, 1024, 0);
        if (client_socketfd < 0) {
            printf("Error: recv()\n");
            break;
        }
        if (result_recv > 0) {
            buffer[result_recv] = 0;
            printf("%s\n", buffer);
        }
        if (result_recv == 0) break;
    }

    close(client_socket->acceptedSocketfd);
    shutdown(server_socketfd, SHUT_RDWR);

    return 0;
}

struct sockaddr_in * createAddr(char *ip, int port){
    struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    inet_pton(AF_INET, ip, &(addr->sin_addr.s_addr));
    return addr;
}

struct AcceptedConnection * acceptConnection(int server_socketfd) {
    struct AcceptedConnection * acceptedConn = malloc(sizeof(struct AcceptedConnection));
    struct sockaddr_in clientAddr;
    int clientAddrSize = sizeof(struct sockaddr_in);

    int client_socketfd = accept(server_socketfd, &clientAddr, &clientAddrSize);
    if (client_socketfd < 0) printf("Error: acceptConnection()\n");

    acceptedConn->addr = clientAddr;
    acceptedConn->acceptedSocketfd = client_socketfd;
    acceptedConn->status = (client_socketfd > 0) ? 1 : 0; 
    if(acceptedConn->status == 0) acceptedConn->error = client_socketfd;

    return acceptedConn;
}