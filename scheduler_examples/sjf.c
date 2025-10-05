#include "sjf.h"

#include <stdio.h>
#include <stdlib.h>

#include "msg.h"
#include <unistd.h>

void sjf_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
    if (*cpu_task) {
        (*cpu_task)->ellapsed_time_ms += TICKS_MS;
        if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) {
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

    if (*cpu_task == NULL && rq->head) {
        // Encontrar o PCB com menor tempo restante
        queue_elem_t *it = rq->head;
        queue_elem_t *shortest_elem = it;
        queue_elem_t *prev = NULL, *prev_shortest = NULL;

        while (it->next) {
            if (it->next->pcb->time_ms < shortest_elem->pcb->time_ms) {
                prev_shortest = it;
                shortest_elem = it->next;
            }
            it = it->next;
        }

        // Remover da fila
        if (shortest_elem == rq->head) {
            *cpu_task = dequeue_pcb(rq);
        } else {
            if (prev_shortest) {
                prev_shortest->next = shortest_elem->next;
                if (shortest_elem == rq->tail) rq->tail = prev_shortest;
                *cpu_task = shortest_elem->pcb;
                free(shortest_elem);
            }
        }
    }
}