#include "../include/algorithms.h"
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>

/* Helper: copy and reset dynamic fields */
static process_t *copy_procs(const process_t *src, int n){
    process_t *p = malloc(sizeof(process_t)*n);
    for(int i=0;i<n;i++) p[i] = src[i];
    return p;
}

static void reset_procs(process_t *p, int n){
    for(int i=0;i<n;i++){
        p[i].remaining_time = p[i].burst_time;
        p[i].start_time = -1;
        p[i].completion_time = -1;
        p[i].turnaround_time = 0;
        p[i].waiting_time = 0;
        p[i].response_time = -1;
    }
}

static int append_event(timeline_event_t *timeline, int tcount, int time, int pid, int duration){
    timeline[tcount].time = time;
    timeline[tcount].pid = pid;
    timeline[tcount].duration = duration;
    return tcount+1;
}

void schedule_fifo(process_t *processes, int n, timeline_event_t *timeline){
    process_t *p = copy_procs(processes, n);
    reset_procs(p, n);
    int t = 0, tcount = 0, completed = 0;
    // sort by arrival then pid
    for(int i=0;i<n-1;i++) for(int j=i+1;j<n;j++){
        if(p[i].arrival_time > p[j].arrival_time || (p[i].arrival_time==p[j].arrival_time && p[i].pid>p[j].pid)){
            process_t tmp = p[i]; p[i]=p[j]; p[j]=tmp;
        }
    }
    while(completed < n){
        int idx = -1;
        for(int i=0;i<n;i++) if(p[i].completion_time==-1 && p[i].arrival_time <= t){ idx = i; break; }
        if(idx==-1){ t++; continue; }
        process_t *pr = &p[idx];
        if(pr->start_time==-1) pr->start_time = t;
        tcount = append_event(timeline, tcount, t, pr->pid, pr->remaining_time);
        t += pr->remaining_time;
        pr->remaining_time = 0;
        pr->completion_time = t;
        pr->turnaround_time = pr->completion_time - pr->arrival_time;
        pr->waiting_time = pr->turnaround_time - pr->burst_time;
        pr->response_time = pr->start_time - pr->arrival_time;
        completed++;
    }
    // copy results back to original array
    for(int i=0;i<n;i++){
        // find matching pid in p
        for(int j=0;j<n;j++) if(processes[i].pid == p[j].pid){
            processes[i] = p[j]; break;
        }
    }
    free(p);
}

void schedule_sjf(process_t *processes, int n, timeline_event_t *timeline){
    process_t *p = copy_procs(processes, n);
    reset_procs(p, n);
    int t = 0, tcount = 0, completed = 0;
    while(completed < n){
        int best = -1, best_burst = INT_MAX;
        for(int i=0;i<n;i++){
            if(p[i].completion_time!=-1) continue;
            if(p[i].arrival_time <= t){
                if(p[i].burst_time < best_burst || (p[i].burst_time==best_burst && p[i].pid < p[best].pid)){
                    best = i; best_burst = p[i].burst_time;
                }
            }
        }
        if(best==-1){ t++; continue; }
        process_t *pr = &p[best];
        if(pr->start_time==-1) pr->start_time = t;
        tcount = append_event(timeline, tcount, t, pr->pid, pr->remaining_time);
        t += pr->remaining_time;
        pr->remaining_time = 0;
        pr->completion_time = t;
        pr->turnaround_time = pr->completion_time - pr->arrival_time;
        pr->waiting_time = pr->turnaround_time - pr->burst_time;
        pr->response_time = pr->start_time - pr->arrival_time;
        completed++;
    }
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++) if(processes[i].pid == p[j].pid){ processes[i] = p[j]; break; }
    }
    free(p);
}

void schedule_stcf(process_t *processes, int n, timeline_event_t *timeline){
    process_t *p = copy_procs(processes, n);
    reset_procs(p, n);
    int t = 0, tcount = 0, completed = 0;
    int last_pid = -1, last_start = 0;
    while(completed < n){
        int best = -1, best_rem = INT_MAX;
        for(int i=0;i<n;i++){
            if(p[i].completion_time!=-1) continue;
            if(p[i].arrival_time <= t){
                if(p[i].remaining_time < best_rem || (p[i].remaining_time==best_rem && p[i].pid < p[best].pid)){
                    best = i; best_rem = p[i].remaining_time;
                }
            }
        }
        if(best==-1){
            if(last_pid!=-1){ tcount = append_event(timeline, tcount, last_start, last_pid, t-last_start); last_pid=-1; }
            t++; continue;
        }
        process_t *pr = &p[best];
        if(pr->start_time==-1) pr->start_time = t;
        if(last_pid==-1){ last_pid = pr->pid; last_start = t; }
        else if(last_pid != pr->pid){ tcount = append_event(timeline, tcount, last_start, last_pid, t-last_start); last_start = t; last_pid = pr->pid; }
        pr->remaining_time -= 1; t++;
        if(pr->remaining_time==0){ pr->completion_time = t; pr->turnaround_time = pr->completion_time - pr->arrival_time; pr->waiting_time = pr->turnaround_time - pr->burst_time; pr->response_time = pr->start_time - pr->arrival_time; completed++; }
    }
    if(last_pid!=-1) tcount = append_event(timeline, tcount, last_start, last_pid, 0);
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++) if(processes[i].pid == p[j].pid){ processes[i] = p[j]; break; }
    }
    free(p);
}

void schedule_rr(process_t *processes, int n, int quantum, timeline_event_t *timeline){
    if(quantum <= 0) quantum = 1;
    process_t *p = copy_procs(processes, n);
    reset_procs(p, n);
    int t=0, tcount=0, completed=0;
    int cap = n*4; int *queue = malloc(sizeof(int)*cap); int qh=0, qt=0;
    for(int i=0;i<n;i++) if(p[i].arrival_time==0) queue[qt++]=i;
    while(completed < n){
        if(qh==qt){
            int next_arr = INT_MAX; for(int i=0;i<n;i++) if(p[i].completion_time==-1 && p[i].arrival_time>t) next_arr = (p[i].arrival_time < next_arr)? p[i].arrival_time : next_arr;
            if(next_arr==INT_MAX) break;
            t = next_arr;
            for(int i=0;i<n;i++) if(p[i].arrival_time==t) queue[qt++]=i;
            continue;
        }
        int idx = queue[qh++]; if(qh==cap) qh=0;
        process_t *pr = &p[idx];
        if(pr->start_time==-1) pr->start_time = t;
        int run = (pr->remaining_time < quantum)? pr->remaining_time : quantum;
        tcount = append_event(timeline, tcount, t, pr->pid, run);
        pr->remaining_time -= run; t += run;
        for(int i=0;i<n;i++) if(p[i].arrival_time>t-run && p[i].arrival_time<=t && p[i].completion_time==-1) queue[qt++]=i;
        if(pr->remaining_time==0){ pr->completion_time = t; pr->turnaround_time = pr->completion_time - pr->arrival_time; pr->waiting_time = pr->turnaround_time - pr->burst_time; pr->response_time = pr->start_time - pr->arrival_time; completed++; }
        else { queue[qt++]=idx; if(qt==cap) qt=0; }
    }
    free(queue);
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++) if(processes[i].pid == p[j].pid){ processes[i] = p[j]; break; }
    }
    free(p);
}

void schedule_mlfq(process_t *processes, int n, mlfq_config_t *config, timeline_event_t *timeline){
    if(config->num_queues <= 0) return;
    process_t *p = copy_procs(processes, n);
    reset_procs(p, n);
    int t=0, tcount=0, completed=0;
    int maxq = config->num_queues; int cap = n*4;
    int **qs = malloc(sizeof(int*)*maxq);
    int *qh = malloc(sizeof(int)*maxq);
    int *qt = malloc(sizeof(int)*maxq);
    for(int i=0;i<maxq;i++){ qs[i]=malloc(sizeof(int)*cap); qh[i]=qt[i]=0; }
    // enqueue helper
    void enqueue(int q, int idx){ qs[q][qt[q]++] = idx; if(qt[q]==cap) qt[q]=0; }
    for(int i=0;i<n;i++) if(p[i].arrival_time==0) enqueue(0,i);
    int next_boost = (config->boost_interval>0)? config->boost_interval : INT_MAX;
    while(completed < n){
        if(t >= next_boost){
            for(int q=1;q<maxq;q++){
                while(qh[q] != qt[q]){ int id = qs[q][qh[q]++]; if(qh[q]==cap) qh[q]=0; enqueue(0, id); }
            }
            next_boost += config->boost_interval;
        }
        int found_q = -1;
        for(int q=0;q<maxq;q++) if(qh[q] != qt[q]){ found_q = q; break; }
        if(found_q==-1){ int next_arr = INT_MAX; for(int i=0;i<n;i++) if(p[i].completion_time==-1 && p[i].arrival_time>t) next_arr = (p[i].arrival_time < next_arr)? p[i].arrival_time : next_arr; if(next_arr==INT_MAX) break; t = next_arr; for(int i=0;i<n;i++) if(p[i].arrival_time==t) enqueue(0,i); continue; }
        int qsel = found_q; int idx = qs[qsel][qh[qsel]++]; if(qh[qsel]==cap) qh[qsel]=0;
        process_t *pr = &p[idx];
        if(pr->start_time==-1) pr->start_time = t;
        int quantum = config->quantums[qsel]; if(quantum<=0) quantum=1;
        int run = (pr->remaining_time < quantum)? pr->remaining_time : quantum;
        tcount = append_event(timeline, tcount, t, pr->pid, run);
        pr->remaining_time -= run; t += run;
        for(int i=0;i<n;i++) if(p[i].arrival_time>t-run && p[i].arrival_time<=t && p[i].completion_time==-1) enqueue(0,i);
        if(pr->remaining_time==0){ pr->completion_time = t; pr->turnaround_time = pr->completion_time - pr->arrival_time; pr->waiting_time = pr->turnaround_time - pr->burst_time; pr->response_time = pr->start_time - pr->arrival_time; completed++; }
        else { if(run==quantum && qsel < maxq-1) enqueue(qsel+1, idx); else enqueue(qsel, idx); }
    }
    for(int i=0;i<maxq;i++) free(qs[i]); free(qs); free(qh); free(qt);
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++) if(processes[i].pid == p[j].pid){ processes[i] = p[j]; break; }
    }
    free(p);
}
