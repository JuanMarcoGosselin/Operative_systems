#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/algorithms.h"
#include "../include/metrics.h"

/* CLI launcher / entry point
 * If run with `--gui` it will start the GTK GUI (gui_gtk_main in gui_gtk.c)
 * Otherwise it will run a small CLI example using FIFO.
 */

extern int gui_gtk_main(int argc, char **argv); // implemented in gui_gtk.c

int main(int argc, char **argv){
    if(argc > 1 && strcmp(argv[1], "--gui") == 0){
        return gui_gtk_main(argc, argv);
    }

    // Simple CLI demo
    process_t procs[] = {
        {1, 0, 5, 1, 0, -1, -1, 0, 0, -1},
        {2, 1, 3, 2, 0, -1, -1, 0, 0, -1},
        {3, 2, 8, 1, 0, -1, -1, 0, 0, -1}
    };
    int n = sizeof(procs)/sizeof(procs[0]);
    timeline_event_t timeline[1024];
    for(int i=0;i<1024;i++) timeline[i].time = timeline[i].pid = timeline[i].duration = 0;

    printf("Running FIFO (CLI demo) on 3-process workload...\n");
    schedule_fifo(procs, n, timeline);

    printf("Gantt (timeline events):\n");
    for(int i=0;i<20;i++){
        if(timeline[i].duration==0) break;
        printf("time %d: P%d ran for %d\n", timeline[i].time, timeline[i].pid, timeline[i].duration);
    }

    // compute metrics (approx total_time = max completion)
    int total_time = 0;
    for(int i=0;i<n;i++) if(procs[i].completion_time > total_time) total_time = procs[i].completion_time;
    metrics_t m;
    calculate_metrics(procs, n, total_time, &m);
    printf("Avg TAT: %.2f, Avg WT: %.2f, Avg RT: %.2f, CPU Util: %.2f%%\n",
           m.avg_turnaround_time, m.avg_waiting_time, m.avg_response_time, m.cpu_utilization);

    printf("To run GUI: ./build/bin/scheduler --gui\n");
    return 0;
}
