// server.c

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define maxClients 10

struct sockaddr_in * createAddr(char *ip, int port);
void handler(int server_socketfd);
void serverBroadcast(char *msg, int size);
void clientBroadcast(char *msg, int client_socketfd, char *name);

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct AcceptedConnection {
    int acceptedSocketfd;
    struct sockaddr_in addr;
    int error;
    char status;
    char name[20];
};

struct AcceptedConnection connectedClients[maxClients];
int connectedClientCount = 0;

struct AcceptedConnection * acceptConnection(int server_socketfd);
void startReceiver(struct AcceptedConnection * acceptedConn);
void freeSocket(struct AcceptedConnection *socketfd);
void startReceiver_pt(struct AcceptedConnection * acceptedConn);

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
    
    handler(server_socketfd);

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
    acceptedConn->status = (client_socketfd >= 0) ? 1 : 0; 
    if(acceptedConn->status == 0) acceptedConn->error = client_socketfd;

    return acceptedConn;
}

void startReceiver_pt(struct AcceptedConnection * acceptedConn) {
    int socketfd = acceptedConn->acceptedSocketfd;
    char name[20];
    ssize_t result_recv_name = recv(socketfd, name, 20, 0);
    if (result_recv_name < 0) {
            printf("Error: name recv()\n");
        }
    if (result_recv_name > 0) {
        strcpy(acceptedConn->name, name);

        char buffer[1024];
        while(1) {
            ssize_t result_recv = recv(socketfd, buffer, 1024, 0);
            if (result_recv < 0) {
                printf("Error: msg recv()\n");
                break;
            }
            if (result_recv > 0) {
                buffer[result_recv] = 0;
                clientBroadcast(buffer, socketfd, acceptedConn->name);
            }
            if (result_recv == 0) break;
        }
    }
    char *err = "Error: Invalid username.";
    send(socketfd, err, sizeof(err), 0);
    close(socketfd);
    freeSocket(acceptedConn);
}

void handler(int server_socketfd) {
    while(1) {
        struct AcceptedConnection *client_socket = acceptConnection(server_socketfd);
        if(client_socket->status){
            pthread_mutex_lock(&lock);
            if (connectedClientCount < maxClients) {
                connectedClients[connectedClientCount++] = *client_socket;
            }
            pthread_mutex_unlock(&lock);
        }
        startReceiver(client_socket);
    }
}

void startReceiver(struct AcceptedConnection * acceptedConn) {
    pthread_t id;
    pthread_create(&id, NULL, (void *)startReceiver_pt, (void *)acceptedConn);
}

void serverBroadcast(char *msg, int size) {
    pthread_mutex_lock(&lock);
    for(int i = 0; i < connectedClientCount; i++) {
        send(connectedClients[i].acceptedSocketfd, msg, size, 0);
    }
    pthread_mutex_unlock(&lock);
}

void clientBroadcast(char *msg, int client_socketfd, char *name) {
    pthread_mutex_lock(&lock);
    char *buffer[1024];
    sprintf(buffer, "%s: %s", name, msg);
    int size = strlen(buffer);
    for(int i = 0; i < connectedClientCount; i++) {
        if(connectedClients[i].acceptedSocketfd == client_socketfd) continue;
        printf("%s\n", buffer);
        send(connectedClients[i].acceptedSocketfd, buffer, size, 0);
    }
    pthread_mutex_unlock(&lock);
}

void freeSocket(struct AcceptedConnection *acceptedConn) {
    pthread_mutex_lock(&lock);
    for(int i = 0; i < connectedClientCount; i++) {
        if(connectedClients[i].acceptedSocketfd == acceptedConn->acceptedSocketfd) {
            connectedClientCount--;
            connectedClients[i] = connectedClients[connectedClientCount];
            free(acceptedConn);
        }
    }
    pthread_mutex_unlock(&lock);
}