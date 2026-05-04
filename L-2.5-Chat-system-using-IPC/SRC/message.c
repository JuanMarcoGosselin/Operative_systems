#include "message.h"
#include <string.h>
#include <time.h>

void message_init(message_t *m, message_type_t type, const char *from, const char *to, const char *text) {
    if(!m) return;
    m->type = type;
    m->timestamp = time(NULL);
    m->client_id = -1;
    if(from) strncpy(m->from_user, from, USERNAME_MAX-1); else m->from_user[0]=0;
    if(to) strncpy(m->to_user, to, USERNAME_MAX-1); else m->to_user[0]=0;
    if(text) strncpy(m->text, text, MSG_MAX_SIZE-1); else m->text[0]=0;
}


