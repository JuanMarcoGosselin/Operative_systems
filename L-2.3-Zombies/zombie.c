#include "zombie.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>

static zombie_stats_t stats;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static void sigchld_handler(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        pthread_mutex_lock(&lock);
        stats.zombies_reaped++;
        stats.zombies_active--;
        pthread_mutex_unlock(&lock);
    }
}

void zombie_init(void) {
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);
}

pid_t zombie_safe_fork(void) {
    pid_t pid = fork();
    if (pid > 0) {
        pthread_mutex_lock(&lock);
        stats.zombies_created++;
        stats.zombies_active++;
        pthread_mutex_unlock(&lock);
    }
    return pid;
}

int zombie_safe_spawn(const char *command, char *args[]) {
    pid_t pid = zombie_safe_fork();
    if (pid == 0) {
        execvp(command, args);
        _exit(1);
    }
    return pid;
}

void zombie_get_stats(zombie_stats_t *out) {
    pthread_mutex_lock(&lock);
    memcpy(out, &stats, sizeof(zombie_stats_t));
    pthread_mutex_unlock(&lock);
}
