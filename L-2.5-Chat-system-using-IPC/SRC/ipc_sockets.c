// ipc_sockets.c
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "ipc.h"

int ipc_init_server_unix(const char *path) {
    int server_fd;
    struct sockaddr_un addr;
    unlink(path);

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path)-1);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        close(server_fd);
        return -1;
    }
    return server_fd;
}

int ipc_accept_unix(int server_fd) {
    int fd = accept(server_fd, NULL, NULL);
    if (fd < 0) perror("accept");
    return fd;
}

int ipc_connect_unix(const char *path) {
    int fd;
    struct sockaddr_un addr;
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path)-1);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(fd);
        return -1;
    }
    return fd;
}

ssize_t ipc_send_fd(int fd, const void *buf, size_t len) {
    return send(fd, buf, len, 0);
}
ssize_t ipc_recv_fd(int fd, void *buf, size_t len) {
    return recv(fd, buf, len, 0);
}


