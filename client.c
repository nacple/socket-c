#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct sockaddr_in * createAddr(char *ip, int port);

int main() {
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in * addr = createAddr("127.0.0.1", 9999);

    int result = connect(socketfd, addr, sizeof(*addr));
    if (result < 0) {
        printf("Error: connect()");
    } printf("Connected!");
    
    return 0;
}

struct sockaddr_in * createAddr(char *ip, int port){
    struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    inet_pton(AF_INET, ip, addr->sin_addr.s_addr);
    return addr;
}