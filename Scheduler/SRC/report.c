#include <stdio.h>
#include "../include/algorithms.h"

void generate_report(const char *filename, process_t *processes, int n){
    FILE *f = fopen(filename, "w");
    if(!f) return;
    fprintf(f, "# Scheduler Performance Report\n\n");
    fprintf(f, "## Process Set\n\n");
    fprintf(f, "| PID | Arrival | Burst | Priority |\n");
    fprintf(f, "|-----|---------|-------|----------|\n");
    for(int i=0;i<n;i++) fprintf(f, "| %d | %d | %d | %d |\n", processes[i].pid, processes[i].arrival_time, processes[i].burst_time, processes[i].priority);
    fclose(f);
}

