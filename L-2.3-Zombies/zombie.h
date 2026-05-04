#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

void reap_explicit(void) {
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
    printf("Explicit reaping done.\n");
}

void sigchld_handler(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
}

void setup_auto_reaper(void) {
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);
    printf("SIGCHLD handler reaper set up.\n");
}

void setup_ignore_reaper(void) {
    signal(SIGCHLD, SIG_IGN);
    printf("SIGCHLD ignored (auto-reap by kernel).\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <strategy>\n", argv[0]);
        return 1;
    }

    int strategy = atoi(argv[1]);
    srand(time(NULL));

    for (int i = 0; i < 10; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            sleep(rand() % 3);
            exit(i);
        }
    }

    switch (strategy) {
        case 1: reap_explicit(); break;
        case 2: setup_auto_reaper(); break;
        case 3: setup_ignore_reaper(); break;
        default: fprintf(stderr, "Invalid strategy\n"); return 1;
    }

    sleep(5);
    system("ps aux | grep defunct | grep -v grep");
    return 0;
}

