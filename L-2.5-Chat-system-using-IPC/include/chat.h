#ifndef CHAT_H
#define CHAT_H

#include "message.h"

#define UNIX_SOCKET_PATH "/tmp/chat.sock"
#define SERVER_PORT 8888
#define MAX_CLIENTS 100
#define BACKLOG 10

typedef struct client {
    int fd;
    char username[USERNAME_MAX];
    time_t last_seen;
    int id;
    struct client *next;
} client_t;

/* server functions */
void server_run_unix_socket(const char *socket_path);
void server_shutdown();

#endif // CHAT_H

