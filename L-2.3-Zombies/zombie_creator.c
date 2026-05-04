#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int create_zombies(int count) {
    for (int i = 0; i < count; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            return -1;
        } else if (pid == 0) {
            exit(i);
        } else {
            printf("Created zombie: PID %d (exit code %d)\n", pid, i);
        }
    }
    printf("Press Enter to exit and clean up zombies...\n");
    getchar();
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <num_zombies>\n", argv[0]);
        return 1;
    }

    int count = atoi(argv[1]);
    if (count <= 0) {
        fprintf(stderr, "Invalid number of zombies.\n");
        return 1;
    }

    return create_zombies(count);
}

