#include <stdio.h>
#include <assert.h>
#include "../include/algorithms.h"

void test_sjf() {
    process_t procs[3] = {
        {1, 0, 8, 1, 0, 0, 0, 0, 0, 0},
        {2, 0, 3, 1, 0, 0, 0, 0, 0, 0},
        {3, 0, 5, 1, 0, 0, 0, 0, 0, 0}
    };
    timeline_event_t timeline[10] = {0};
    int n = 3;

    schedule_sjf(procs, n, timeline);

    assert(procs[0].pid == 2);
    assert(procs[1].pid == 3);
    assert(procs[2].pid == 1);

    printf("test_sjf passed\n");
}

int main() {
    test_sjf();
    return 0;
}
