#ifndef SHARED_H
#define SHARED_H

#define MAX_LIVES 3
#define POS_LEFT 0
#define POS_MIDDLE 1
#define POS_RIGHT 2
#define MAX_NAME_LENGTH 50

#define SERVER_PIPE "server_pipe"
#define CLIENT_PIPE "client_pipe_%s"

#define MAX_PLAYERS 10
#include <semaphore.h>

struct PlayerState{
    char name[MAX_NAME_LENGTH];
    int position; //Left, right, or middle based on defined position constants
    int lives; //Number of lives left
    int score; //For if multiple rounds played
};

struct Lobby{
  char players[MAX_PLAYERS][MAX_NAME_LENGTH];
  int playercount;
  sem_t lobbylock;
};

#endif