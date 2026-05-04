// chat_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "include/ipc.h"
#include "include/message.h"

int sockfd = -1;
char username[USERNAME_MAX];

void *reader_thread(void *arg) {
    (void)arg;
    while (1) {
        message_t m;
        ssize_t s = recv(sockfd, &m, sizeof(m), 0);
        if (s <= 0) {
            printf("[SYSTEM] Disconnected from server\n");
            exit(0);
        }
        if (m.type == MSG_BROADCAST) {
            printf("[%s] %s\n", m.from_user, m.text);
        } else if (m.type == MSG_PRIVATE) {
            if (strcmp(m.to_user, username) == 0) {
                printf("[%s -> you] %s\n", m.from_user, m.text);
            } else {
                // echo
                printf("[You -> %s] %s\n", m.to_user, m.text);
            }
        } else if (m.type == MSG_USER_LIST) {
            printf("[SYSTEM] Users online: %s\n", m.text);
        } else if (m.type == MSG_JOIN) {
            printf("[SYSTEM] %s joined\n", m.from_user);
        } else if (m.type == MSG_LEAVE) {
            printf("[SYSTEM] %s left\n", m.from_user);
        } else if (m.type == MSG_ERROR) {
            printf("[ERROR] %s\n", m.text);
        }
    }
    return NULL;
}

void usage() {
    printf("Commands:\n");
    printf("/help - show this\n");
    printf("/users - list online users\n");
    printf("/msg <user> <message> - private message\n");
    printf("/quit - quit\n");
}

int main() {
    printf("=== Chat Client ===\n");
    printf("Username: ");
    if (!fgets(username, sizeof(username), stdin)) return 1;
    username[strcspn(username, "\n")] = 0;
    if (strlen(username) == 0) {
        printf("Empty username\n");
        return 1;
    }

    sockfd = ipc_connect_unix(UNIX_SOCKET_PATH);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to connect to server\n");
        return 1;
    }

    // send join message
    message_t m;
    memset(&m,0,sizeof(m));
    m.type = MSG_JOIN;
    strncpy(m.from_user, username, USERNAME_MAX-1);
    send(sockfd, &m, sizeof(m), 0);

    printf("Connected to server!\n[SYSTEM] Welcome %s!\n", username);

    pthread_t tid;
    pthread_create(&tid, NULL, reader_thread, NULL);

    char line[512];
    usage();
    while (1) {
        printf("%s> ", username);
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n")] = 0;
        if (strncmp(line, "/quit", 5) == 0) {
            message_t q;
            memset(&q,0,sizeof(q));
            q.type = MSG_LEAVE;
            strncpy(q.from_user, username, USERNAME_MAX-1);
            send(sockfd, &q, sizeof(q), 0);
            close(sockfd);
            printf("[SYSTEM] Goodbye!\n");
            return 0;
        } else if (strncmp(line, "/help", 5) == 0) {
            usage();
        } else if (strncmp(line, "/users", 6) == 0) {
            message_t req;
            memset(&req,0,sizeof(req));
            req.type = MSG_LIST_USERS;
            strncpy(req.from_user, username, USERNAME_MAX-1);
            send(sockfd, &req, sizeof(req), 0);
        } else if (strncmp(line, "/msg ", 5) == 0) {
            char target[USERNAME_MAX];
            char *p = line + 5;
            if (sscanf(p, "%31s", target) == 1) {
                // find message start
                char *msgtext = strchr(p, ' ');
                if (msgtext) {
                    msgtext = strchr(msgtext+1, ' ');
                    if (msgtext) msgtext++;
                }
                if (!msgtext) {
                    printf("Usage: /msg <user> <message>\n");
                    continue;
                }
                message_t pm;
                memset(&pm,0,sizeof(pm));
                pm.type = MSG_PRIVATE;
                strncpy(pm.from_user, username, USERNAME_MAX-1);
                strncpy(pm.to_user, target, USERNAME_MAX-1);
                strncpy(pm.text, msgtext, MSG_MAX_SIZE-1);
                send(sockfd, &pm, sizeof(pm), 0);
            } else {
                printf("Usage: /msg <user> <message>\n");
            }
        } else {
            // broadcast
            message_t bm;
            memset(&bm,0,sizeof(bm));
            bm.type = MSG_BROADCAST;
            strncpy(bm.from_user, username, USERNAME_MAX-1);
            strncpy(bm.text, line, MSG_MAX_SIZE-1);
            send(sockfd, &bm, sizeof(bm), 0);
        }
    }
    return 0;
}

