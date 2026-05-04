#include <stdio.h>
#include "../include/algorithms.h"
#include "../include/metrics.h"

int main() {
    process_t procs[3] = {
        {1, 0, 5, 1, 0, -1, -1, 0, 0, -1},
        {2, 1, 3, 1, 0, -1, -1, 0, 0, -1},
        {3, 2, 8, 1, 0, -1, -1, 0, 0, -1}
    };
    int n = 3;
    timeline_event_t timeline[20] = {0};

    schedule_fifo(procs, n, timeline);

    // Calcular métricas
    int total_time = 0;
    for(int i=0;i<n;i++){
        if(procs[i].completion_time > total_time)
            total_time = procs[i].completion_time;
    }

    metrics_t m;
    calculate_metrics(procs, n, total_time, &m);

    // Imprimir métricas
    printf("Algorithm: FIFO\n");
    for(int i=0;i<n;i++){
        printf("PID %d: Turnaround=%d, Waiting=%d, Response=%d\n",
            procs[i].pid,
            procs[i].turnaround_time,
            procs[i].waiting_time,
            procs[i].response_time);
    }
    printf("CPU Utilization: %.2f%%\n", m.cpu_utilization);
    printf("Throughput: %.2f\n", m.throughput);
    printf("Fairness Index: %.2f\n", m.fairness_index);

    return 0;
}
