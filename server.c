#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>

struct sockaddr_in * createAddr(char *ip, int port);

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

    struct sockaddr_in clientAddr;
    int clientAddrSize = sizeof(struct sockaddr_in);

    int client_socketfd = accept(server_socketfd, &clientAddr, &clientAddrSize);
    if (client_socketfd < 0) {
        printf("Error: accept()\n");
        return 1;
    }

    char buffer[1024];
    recv(client_socketfd, buffer, 1024, 0);
    printf("%s\n", buffer);

    return 0;
}

struct sockaddr_in * createAddr(char *ip, int port){
    struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    inet_pton(AF_INET, ip, addr->sin_addr.s_addr);
    return addr;
}