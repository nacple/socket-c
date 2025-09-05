#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

struct sockaddr_in * createAddr(char *ip, int port);

int main() {
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in * addr = createAddr("127.0.0.1", 9888);

    int result = connect(socketfd, addr, sizeof(*addr));
    if (result < 0) {
        printf("Error: connect()\n");
        return 1;
    } printf("Connected!\n");

    char *line = NULL;
    size_t lineSize = 0;
    printf("Message: ");
    
    while (1)
    {
        ssize_t charCount = getline(&line, &lineSize, stdin);
        if (charCount > 0) {
            if (strcmp(line, "exit\n") == 0) break;
            ssize_t result_send = send(socketfd, line, charCount, 0);
        }
    }

    close(socketfd);

    return 0;
}

struct sockaddr_in * createAddr(char *ip, int port){
    struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    inet_pton(AF_INET, ip, &(addr->sin_addr.s_addr));
    return addr;
}