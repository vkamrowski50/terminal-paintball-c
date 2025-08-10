#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include "shared.h"

#define PORT "9999"
#define BUFFER_SIZE 256
#define MAX_LIVES 3

struct Player {
    int position;
    int lives;
};

void handle_game(int client1, int client2, int client3, int client4) {
    char buffer[BUFFER_SIZE];
    struct Player players[4] = {
        {POS_MIDDLE, MAX_LIVES},
        {POS_MIDDLE, MAX_LIVES},
        {POS_MIDDLE, MAX_LIVES},
        {POS_MIDDLE, MAX_LIVES}
    };
    int active_clients[4] = {client1, client2, client3, client4};
    int player_count = 4;

    while (1) {
        for (int current_player = 0; current_player < 4; current_player++) {
            if (players[current_player].lives <= 0 || active_clients[current_player] == -1) {
                continue; // Skip players who are dead or have quit
            }

            int active_client = active_clients[current_player];

            while (1) { // Ensure the player's turn only ends with a valid command
                snprintf(buffer, sizeof(buffer),
                         "Your turn. Lives: %d\nEnter command (POSITION <0|1|2> or SHOOT <player_id> <0|1|2>) (type 'quit' to exit): ",
                         players[current_player].lives);
                send(active_client, buffer, strlen(buffer), 0);

                memset(buffer, 0, sizeof(buffer));
                int bytes_read = recv(active_client, buffer, sizeof(buffer) - 1, 0);
                if (bytes_read <= 0 || strncmp(buffer, "quit", 4) == 0) {
                    // Player has quit
                    snprintf(buffer, sizeof(buffer), "Player %d has left the game.\n", current_player + 1);
                    for (int i = 0; i < 4; i++) {
                        if (active_clients[i] != -1 && i != current_player) {
                            send(active_clients[i], buffer, strlen(buffer), 0);
                        }
                    }
                    close(active_client);
                    active_clients[current_player] = -1;
                    players[current_player].lives = 0;
                    player_count--;

                    // Check for game over due to forfeit
                    if (player_count == 1) {
                        for (int i = 0; i < 4; i++) {
                            if (active_clients[i] != -1) {
                                snprintf(buffer, sizeof(buffer), "Player %d wins by forfeit! Game over.\n", i + 1);
                                send(active_clients[i], buffer, strlen(buffer), 0);
                            }
                        }
                        return;
                    }
                    break; // Exit the turn loop
                }

                buffer[bytes_read] = '\0';

                if (strncmp(buffer, "POSITION", 8) == 0) {
                    int new_position;
                    if (sscanf(buffer + 9, "%d", &new_position) == 1 && new_position >= 0 && new_position <= 2) {
                        players[current_player].position = new_position;
                        snprintf(buffer, sizeof(buffer), "Player %d moved to position %d\n", current_player + 1, new_position);
                        send(active_client, buffer, strlen(buffer), 0);
                        break; // End the turn after a valid move
                    } else {
                        snprintf(buffer, sizeof(buffer), "Invalid position. Use 0 (left), 1 (middle), or 2 (right)\n");
                        send(active_client, buffer, strlen(buffer), 0);
                        continue; // Prompt the same player again
                    }
                } else if (strncmp(buffer, "SHOOT", 5) == 0) {
                    int target_player, target_position;
                    if (sscanf(buffer + 6, "%d %d", &target_player, &target_position) == 2 &&
                        target_player >= 1 && target_player <= 4 && target_position >= 0 && target_position <= 2) {
                        target_player--; // Convert to 0-index

                        if (target_player == current_player || players[target_player].lives <= 0 || active_clients[target_player] == -1) {
                            snprintf(buffer, sizeof(buffer), "Invalid target.\n");
                            send(active_client, buffer, strlen(buffer), 0);
                            continue; // Prompt the same player again
                        }

                        if (target_position == players[target_player].position) {
                            players[target_player].lives--;
                            snprintf(buffer, sizeof(buffer), "Player %d hit Player %d! Remaining lives: %d\n",
                                     current_player + 1, target_player + 1, players[target_player].lives);
                        } else {
                            snprintf(buffer, sizeof(buffer), "Player %d missed Player %d.\n", current_player + 1, target_player + 1);
                        }

                        for (int i = 0; i < 4; i++) {
                            if (active_clients[i] != -1) {
                                send(active_clients[i], buffer, strlen(buffer), 0);
                            }
                        }

                        // Check for game over
                        int alive_count = 0, winner = -1;
                        for (int i = 0; i < 4; i++) {
                            if (players[i].lives > 0 && active_clients[i] != -1) {
                                alive_count++;
                                winner = i;
                            }
                        }

                        if (alive_count == 1) {
                            snprintf(buffer, sizeof(buffer), "Player %d wins! Game over.\n", winner + 1);
                            for (int i = 0; i < 4; i++) {
                                if (active_clients[i] != -1) {
                                    send(active_clients[i], buffer, strlen(buffer), 0);
                                }
                            }
                            return;
                        }

                        break; // End the turn after a valid shoot
                    } else {
                        snprintf(buffer, sizeof(buffer), "Invalid shoot command. Use SHOOT <player_id> <0|1|2>\n");
                        send(active_client, buffer, strlen(buffer), 0);
                        continue; // Prompt the same player again
                    }
                } else {
                    snprintf(buffer, sizeof(buffer), "Invalid command! Use POSITION or SHOOT.\n");
                    send(active_client, buffer, strlen(buffer), 0);
                    continue; // Prompt the same player again
                }
            }
        }
    }
}

int main() {
    struct addrinfo hints, *res;
    int listen_socket, client1, client2, client3, client4;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) {
        perror("getaddrinfo failed");
        exit(1);
    }

    listen_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (listen_socket == -1) {
        perror("socket failed");
        exit(1);
    }

    if (bind(listen_socket, res->ai_addr, res->ai_addrlen) == -1) {
        perror("bind failed");
        close(listen_socket);
        exit(1);
    }

    freeaddrinfo(res);

    if (listen(listen_socket, 4) == -1) {
        perror("listen failed");
        close(listen_socket);
        exit(1);
    }

    printf("Server listening on port %s...\n", PORT);

    while (1) {
        printf("Waiting for players...\n");

        printf("Waiting for Player 1\n");
        client1 = accept(listen_socket, NULL, NULL);
        if (client1 == -1) {
            perror("accept failed");
            continue;
        }
        printf("Player 1 connected\n");

        printf("Waiting for Player 2\n");
        client2 = accept(listen_socket, NULL, NULL);
        if (client2 == -1) {
            perror("accept failed");
            close(client1);
            continue;
        }
        printf("Player 2 connected\n");

        printf("Waiting for Player 3\n");
        client3 = accept(listen_socket, NULL, NULL);
        if (client3 == -1) {
            perror("accept failed");
            close(client1);
            close(client2);
            continue;
        }
        printf("Player 3 connected\n");

        printf("Waiting for Player 4\n");
        client4 = accept(listen_socket, NULL, NULL);
        if (client4 == -1) {
            perror("accept failed");
            close(client1);
            close(client2);
            close(client3);
            continue;
        }
        printf("Player 4 connected\n");

        if (fork() == 0) {
            close(listen_socket);
            handle_game(client1, client2, client3, client4);
            exit(0);
        }

        close(client1);
        close(client2);
        close(client3);
        close(client4);
    }

    close(listen_socket);
    return 0;
}