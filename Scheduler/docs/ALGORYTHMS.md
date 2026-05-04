# Scheduling Algorithms

## 1. First In First Out (FIFO)
- **Type:** Non-preemptive
- **Idea:** Processes execute in order of arrival.
- **Advantages:** Simple, minimal overhead.
- **Disadvantages:** Can cause long waiting times ("convoy effect").
- **Metrics:**
  - Start Time: when the CPU first runs the process.
  - Completion Time = current_time + burst_time
  - Turnaround = Completion - Arrival
  - Waiting = Turnaround - Burst
  - Response = Start - Arrival

---

## 2. Shortest Job First (SJF)
- **Type:** Non-preemptive
- **Idea:** The shortest available job is always chosen.
- **Advantages:** Minimizes average waiting time.
- **Disadvantages:** Starvation possible for long jobs.
- **Implementation:**
  - At each step, pick the ready process with the smallest burst.
  - If none are ready, increment time.

---

## 3. Shortest Time to Completion First (STCF)
- **Type:** Preemptive SJF
- **Idea:** The job with the least remaining time always runs.
- **Mechanism:**
  - Each time unit, select process with smallest remaining time.
  - Preempt current process if a shorter one arrives.
- **Advantages:** Optimal average turnaround time.
- **Disadvantages:** High context-switch overhead.

---

## 4. Round Robin (RR)
- **Type:** Preemptive
- **Idea:** Each process receives equal CPU time slices (quantum).
- **Mechanism:**
  - Use a circular queue.
  - After each quantum, reinsert unfinished process at queue’s end.
- **Advantages:** Fairness, good response time for interactive tasks.
- **Disadvantages:** Depends heavily on quantum size.

---

## 5. Multi-Level Feedback Queue (MLFQ)
- **Type:** Preemptive, multi-queue
- **Idea:** Multiple queues with decreasing priorities and increasing quantums.
- **Rules:**
  - New processes start in top queue.
  - If a process uses full quantum → move down.
  - Periodic **boost** returns all to top queue.
- **Advantages:** Balances I/O-bound (interactive) and CPU-bound jobs.
- **Disadvantages:** Complex tuning.

---

## Metric Formulas

| Metric | Formula | Description |
|:--|:--|:--|
| **Turnaround Time** | `Completion - Arrival` | Total time in system |
| **Waiting Time** | `Turnaround - Burst` | Time spent waiting |
| **Response Time** | `Start - Arrival` | Delay before first CPU access |
| **CPU Utilization** | `(Busy Time / Total Time) × 100` | Efficiency |
| **Throughput** | `Processes Completed / Total Time` | Work done per unit time |
| **Fairness** | `(Σxi)² / (n × Σxi²)` | Balance across processes |
