// ipc_mq.c : example wrapper for POSIX message queues
#include <mqueue.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "message.h"

#define MQ_NAME "/chat_mq"
#define MQ_MAX_MSG 10
#define MQ_MSG_SIZE sizeof(message_t)

mqd_t mq = (mqd_t) -1;

int mq_init_server() {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MQ_MAX_MSG;
    attr.mq_msgsize = MQ_MSG_SIZE;
    attr.mq_curmsgs = 0;

    mq_unlink(MQ_NAME);
    mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        return -1;
    }
    return 0;
}

int mq_send_msg(message_t *m) {
    if (mq_send(mq, (const char*)m, sizeof(message_t), 0) == -1) {
        perror("mq_send");
        return -1;
    }
    return 0;
}

int mq_recv_msg(message_t *m) {
    ssize_t s = mq_receive(mq, (char*)m, sizeof(message_t), NULL);
    if (s == -1) {
        perror("mq_receive");
        return -1;
    }
    return 0;
}

void mq_close_server() {
    if (mq != (mqd_t)-1) {
        mq_close(mq);
        mq_unlink(MQ_NAME);
    }
}

