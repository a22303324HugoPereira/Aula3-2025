#include "mlfq.h"

#include <stdio.h>
#include <stdlib.h>

#include "msg.h"
#include <unistd.h>

#define LEVELS 3
#define QUANTUM_MS_LEVEL {20, 40, 80}

void mlfq_scheduler(uint32_t current_time_ms, queue_t *queues[LEVELS], pcb_t **cpu_task) {
    static int quantums[LEVELS] = QUANTUM_MS_LEVEL;

    if (*cpu_task) {
        (*cpu_task)->ellapsed_time_ms += TICKS_MS;

        int level = 0; // Se não temos priority, assumir nível 0
        if ((*cpu_task)->ellapsed_time_ms >= quantums[level]) {
            int next_level = level + 1;
            if (next_level >= LEVELS) next_level = LEVELS - 1;
            enqueue_pcb(queues[next_level], *cpu_task);
            *cpu_task = NULL;
        } else if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) {
            msg_t msg = {
                .pid = (*cpu_task)->pid,
                .request = PROCESS_REQUEST_DONE,
                .time_ms = current_time_ms
            };
            write((*cpu_task)->sockfd, &msg, sizeof(msg_t));
            free(*cpu_task);
            *cpu_task = NULL;
        }
    }

    if (*cpu_task == NULL) {
        for (int i = 0; i < LEVELS; i++) {
            if (queues[i]->head) {
                *cpu_task = dequeue_pcb(queues[i]);
                break;
            }
        }
    }
}