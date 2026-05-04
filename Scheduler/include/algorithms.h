#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <stddef.h>

typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_time;
    int start_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
    int response_time;
} process_t;

typedef struct {
    int time;
    int pid;
    int duration;
} timeline_event_t;

typedef struct { int num_queues; int *quantums; int boost_interval; } mlfq_config_t;

void schedule_fifo(process_t *processes, int n, timeline_event_t *timeline);
void schedule_sjf(process_t *processes, int n, timeline_event_t *timeline);
void schedule_stcf(process_t *processes, int n, timeline_event_t *timeline);
void schedule_rr(process_t *processes, int n, int quantum, timeline_event_t *timeline);
void schedule_mlfq(process_t *processes, int n, mlfq_config_t *config, timeline_event_t *timeline);

#endif
