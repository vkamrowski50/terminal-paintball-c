#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include "shared.h"

#define PORT "9999"
#define BUFFER_SIZE 256

int main() {
    struct addrinfo hints, *res;
    int socket_fd;
    char buffer[BUFFER_SIZE];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo("127.0.0.1", PORT, &hints, &res) != 0) {
        perror("getaddrinfo failed");
        exit(1);
    }

    socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (socket_fd == -1) {
        perror("socket failed");
        exit(1);
    }

    if (connect(socket_fd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("connect failed");
        close(socket_fd);
        exit(1);
    }

    freeaddrinfo(res);
    printf("Connected to server\n");

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = recv(socket_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read <= 0) {
            printf("Disconnected from server\n");
            break;
        }
        buffer[bytes_read] = '\0';
        printf("%s", buffer);

        if (strstr(buffer, "Your turn") != NULL) {
            printf("Enter your command: ");
            memset(buffer, 0, sizeof(buffer));
            fgets(buffer, sizeof(buffer), stdin);

            send(socket_fd, buffer, strlen(buffer), 0);

            if (strncmp(buffer, "quit", 4) == 0) {
                printf("You quit\n");
                break;
            }
        }
    }

    close(socket_fd);
    return 0;
}