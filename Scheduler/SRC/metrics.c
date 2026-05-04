#include "../include/metrics.h"
#include <stdlib.h>

void calculate_metrics(process_t *processes, int n, int total_time, metrics_t *metrics){
    double sum_tat=0, sum_wt=0, sum_rt=0, busy=0;
    for(int i=0;i<n;i++){
        sum_tat += processes[i].turnaround_time;
        sum_wt += processes[i].waiting_time;
        sum_rt += (processes[i].response_time>=0?processes[i].response_time:0);
        busy += processes[i].burst_time;
    }
    metrics->avg_turnaround_time = (n>0)? sum_tat / n : 0.0;
    metrics->avg_waiting_time = (n>0)? sum_wt / n : 0.0;
    metrics->avg_response_time = (n>0)? sum_rt / n : 0.0;
    metrics->cpu_utilization = (total_time>0)? (busy / total_time) * 100.0 : 0.0;
    metrics->throughput = (total_time>0)? ((double)n / total_time) : 0.0;
    double s1=0,s2=0; for(int i=0;i<n;i++){ double xi=processes[i].turnaround_time; s1+=xi; s2+=xi*xi; }
    metrics->fairness_index = (s2==0 || n==0)?1.0: (s1*s1) / (n * s2);
}
