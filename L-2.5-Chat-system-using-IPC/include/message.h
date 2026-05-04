#ifndef MESSAGE_H
#define MESSAGE_H

#include <time.h>

#define MSG_MAX_SIZE 256
#define USERNAME_MAX 32

typedef enum {
    MSG_JOIN,
    MSG_LEAVE,
    MSG_BROADCAST,
    MSG_PRIVATE,
    MSG_LIST_USERS,
    MSG_USER_LIST,
    MSG_ERROR,
    MSG_PING,
    MSG_PONG
} message_type_t;

typedef struct {
    message_type_t type;
    char from_user[USERNAME_MAX];
    char to_user[USERNAME_MAX];
    char text[MSG_MAX_SIZE];
    time_t timestamp;
    int client_id;
} message_t;

#endif // MESSAGE_H

