#ifndef IPC_H
#define IPC_H

/* This file exposes IPC-specific init and send/recv functions.
   We provide two modules:
    - ipc_sockets.c  : UNIX domain sockets
    - ipc_mq.c       : POSIX message queue example
*/

int ipc_init_server_unix(const char *path);
int ipc_accept_unix(int server_fd);
int ipc_connect_unix(const char *path);
ssize_t ipc_send_fd(int fd, const void *buf, size_t len);
ssize_t ipc_recv_fd(int fd, void *buf, size_t len);

#endif // IPC_H


