#include <stdio.h>
#include "../include/algorithms.h"
#include "../include/metrics.h"

int main() {
    process_t procs[3] = {
        {1, 0, 5, 1, 0, -1, -1, 0, 0, -1},
        {2, 1, 3, 1, 0, -1, -1, 0, 0, -1},
        {3, 2, 7, 1, 0, -1, -1, 0, 0, -1}
    };
    int n = 3;
    timeline_event_t timeline[40] = {0};

    int quantums[] = {2, 4};
    mlfq_config_t config = {2, quantums, 50};

    schedule_mlfq(procs, n, &config, timeline);

    int total_time = 0;
    for(int i=0;i<n;i++)
        if(procs[i].completion_time > total_time)
            total_time = procs[i].completion_time;

    metrics_t m;
    calculate_metrics(procs, n, total_time, &m);

    printf("Algorithm: MLFQ\n\n");
    for(int i=0;i<n;i++){
        printf("Process %d:\n", procs[i].pid);
        printf("  Turnaround Time: %d\n", procs[i].turnaround_time);
        printf("  Waiting Time: %d\n", procs[i].waiting_time);
        printf("  Response Time: %d\n", procs[i].response_time);
    }
    printf("\nOverall Metrics:\n");
    printf("  CPU Utilization: %.2f%%\n", m.cpu_utilization);
    printf("  Throughput: %.2f processes/unit time\n", m.throughput);
    printf("  Fairness (Jain's index): %.2f\n", m.fairness_index);

    return 0;
}
