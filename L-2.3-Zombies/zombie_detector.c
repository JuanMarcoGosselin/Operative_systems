#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

#define MAX_ZOMBIES 1024

int find_zombies(int *zombie_pids, int max_zombies) {
    DIR *dir = opendir("/proc");
    if (!dir) {
        perror("opendir");
        return -1;
    }

    struct dirent *entry;
    int count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (!isdigit(entry->d_name[0]))
            continue;

        char path[256];
        snprintf(path, sizeof(path), "/proc/%s/stat", entry->d_name);
        FILE *fp = fopen(path, "r");
        if (!fp)
            continue;

        int pid, ppid;
        char comm[256], state;
        if (fscanf(fp, "%d (%255[^)]) %c %d", &pid, comm, &state, &ppid) == 4) {
            if (state == 'Z' && count < max_zombies) {
                zombie_pids[count++] = pid;
            }
        }
        fclose(fp);
    }
    closedir(dir);
    return count;
}

void print_zombie_info(int pid) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    FILE *fp = fopen(path, "r");
    if (!fp) return;

    int ppid;
    char comm[256], state;
    fscanf(fp, "%*d (%255[^)]) %c %d", comm, &state, &ppid);
    fclose(fp);

    printf("%-8d %-8d %-15s %c\n", pid, ppid, comm, state);
}

int main(void) {
    int pids[MAX_ZOMBIES];
    int count = find_zombies(pids, MAX_ZOMBIES);

    printf("=== Zombie Process Report ===\n");
    printf("Total Zombies: %d\n\n", count);
    printf("PID      PPID     Command         State\n");
    printf("------   ------   --------------  -----\n");

    for (int i = 0; i < count; i++)
        print_zombie_info(pids[i]);

    return 0;
}

