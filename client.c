#include <sys/socket.h>
#include <arpa/inet.h>

int main() {
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);

    char *ip = "0.0.0.0";
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    inet_pton(AF_INET, ip, &addr.sin_addr.s_addr);

    connect(socketfd, &addr, sizeof(addr));

    return 0;
}