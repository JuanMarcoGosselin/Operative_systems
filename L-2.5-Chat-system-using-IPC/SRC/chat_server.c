// chat_server.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/select.h>
#include <pthread.h>
#include <time.h>
#include "include/chat.h"
#include "include/message.h"
#include "include/ipc.h"

static int server_fd = -1;
static client_t *client_list = NULL;
static int next_client_id = 1;
pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
volatile sig_atomic_t running = 1;
int total_messages = 0;

void log_info(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    printf("[INFO] ");
    vprintf(fmt, ap);
    printf("\n");
    va_end(ap);
}

void log_server(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    printf("[SERVER] ");
    vprintf(fmt, ap);
    printf("\n");
    va_end(ap);
}

void add_client_safe(client_t *c) {
    pthread_mutex_lock(&list_mutex);
    c->next = client_list;
    client_list = c;
    pthread_mutex_unlock(&list_mutex);
}

void remove_client_by_fd(int fd) {
    pthread_mutex_lock(&list_mutex);
    client_t **pp = &client_list;
    while (*pp) {
        if ((*pp)->fd == fd) {
            client_t *to = *pp;
            *pp = to->next;
            close(to->fd);
            log_info("Client '%s' disconnected (FD: %d)", to->username, fd);
            free(to);
            break;
        }
        pp = &(*pp)->next;
    }
    pthread_mutex_unlock(&list_mutex);
}

client_t* find_client_by_name(const char *name) {
    pthread_mutex_lock(&list_mutex);
    client_t *it = client_list;
    while (it) {
        if (strncmp(it->username, name, USERNAME_MAX) == 0) {
            pthread_mutex_unlock(&list_mutex);
            return it;
        }
        it = it->next;
    }
    pthread_mutex_unlock(&list_mutex);
    return NULL;
}

void broadcast_message(const message_t *m) {
    pthread_mutex_lock(&list_mutex);
    client_t *it = client_list;
    while (it) {
        // send message struct serialized (simple)
        ssize_t s = send(it->fd, m, sizeof(message_t), 0);
        (void)s;
        it = it->next;
    }
    pthread_mutex_unlock(&list_mutex);
}

void handle_client_message(client_t *c, message_t *m) {
    total_messages++;
    if (m->type == MSG_BROADCAST) {
        log_info("%s -> ALL: %s", m->from_user, m->text);
        broadcast_message(m);
    } else if (m->type == MSG_PRIVATE) {
        log_info("%s -> %s: %s", m->from_user, m->to_user, m->text);
        client_t *dest = find_client_by_name(m->to_user);
        if (dest) {
            send(dest->fd, m, sizeof(message_t), 0);
            // send back to sender for UI
            send(c->fd, m, sizeof(message_t), 0);
        } else {
            message_t resp;
            memset(&resp,0,sizeof(resp));
            resp.type = MSG_ERROR;
            strncpy(resp.text, "User not found", MSG_MAX_SIZE-1);
            send(c->fd, &resp, sizeof(resp), 0);
        }
    } else if (m->type == MSG_LIST_USERS) {
        // compile list
        char buf[MSG_MAX_SIZE];
        buf[0]=0;
        pthread_mutex_lock(&list_mutex);
        client_t *it = client_list;
        while (it) {
            strncat(buf, it->username, sizeof(buf)-strlen(buf)-2);
            if (it->next) strncat(buf, ",", sizeof(buf)-strlen(buf)-1);
            it = it->next;
        }
        pthread_mutex_unlock(&list_mutex);
        message_t resp;
        memset(&resp,0,sizeof(resp));
        resp.type = MSG_USER_LIST;
        strncpy(resp.text, buf, MSG_MAX_SIZE-1);
        send(c->fd, &resp, sizeof(resp), 0);
    }
}

void graceful_shutdown(int signo) {
    running = 0;
    log_server("Shutdown signal received");
}

void server_run_unix_socket(const char *socket_path) {
    server_fd = ipc_init_server_unix(socket_path);
    if (server_fd < 0) {
        fprintf(stderr, "Failed to init server socket\n");
        return;
    }
    log_server("Chat server started on socket %s", socket_path);
    log_server("Using IPC method: UNIX_SOCKETS");

    fd_set readfds;
    int maxfd = server_fd;

    while (running) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);

        pthread_mutex_lock(&list_mutex);
        client_t *it = client_list;
        while (it) {
            FD_SET(it->fd, &readfds);
            if (it->fd > maxfd) maxfd = it->fd;
            it = it->next;
        }
        pthread_mutex_unlock(&list_mutex);

        struct timeval tv = {1,0}; // 1 second
        int ready = select(maxfd + 1, &readfds, NULL, NULL, &tv);
        if (ready < 0) {
            if (errno == EINTR) continue;
            perror("select");
            break;
        } else if (ready == 0) continue;

        if (FD_ISSET(server_fd, &readfds)) {
            int client_fd = ipc_accept_unix(server_fd);
            if (client_fd >= 0) {
                // Expect username message first
                message_t m;
                ssize_t s = recv(client_fd, &m, sizeof(m), 0);
                if (s <= 0) {
                    close(client_fd);
                } else {
                    client_t *c = calloc(1, sizeof(client_t));
                    c->fd = client_fd;
                    c->id = next_client_id++;
                    c->last_seen = time(NULL);
                    strncpy(c->username, m.from_user, USERNAME_MAX-1);
                    add_client_safe(c);
                    log_info("Client '%s' connected (FD: %d)", c->username, c->fd);
                    // notify others
                    message_t join;
                    memset(&join,0,sizeof(join));
                    join.type = MSG_JOIN;
                    strncpy(join.from_user, c->username, USERNAME_MAX-1);
                    broadcast_message(&join);
                }
            }
        }

        // handle client messages
        pthread_mutex_lock(&list_mutex);
        it = client_list;
        // we copy fds to avoid modify-while-iterating issues
        while (it) {
            client_t *c = it;
            it = it->next;
            if (FD_ISSET(c->fd, &readfds)) {
                message_t m;
                ssize_t s = recv(c->fd, &m, sizeof(m), 0);
                if (s <= 0) {
                    // disconnected
                    remove_client_by_fd(c->fd);
                    // notify
                    message_t leave;
                    memset(&leave,0,sizeof(leave));
                    leave.type = MSG_LEAVE;
                    strncpy(leave.from_user, c->username, USERNAME_MAX-1);
                    broadcast_message(&leave);
                } else {
                    handle_client_message(c, &m);
                }
            }
        }
        pthread_mutex_unlock(&list_mutex);
    }

    // cleanup
    pthread_mutex_lock(&list_mutex);
    client_t *cur = client_list;
    while (cur) {
        client_t *n = cur->next;
        close(cur->fd);
        free(cur);
        cur = n;
    }
    client_list = NULL;
    pthread_mutex_unlock(&list_mutex);

    close(server_fd);
    unlink(UNIX_SOCKET_PATH);
    log_server("Server shutdown complete. Total messages: %d", total_messages);
}

int main(int argc, char **argv) {
    signal(SIGINT, graceful_shutdown);
    signal(SIGTERM, graceful_shutdown);
    server_run_unix_socket(UNIX_SOCKET_PATH);
    return 0;
}

