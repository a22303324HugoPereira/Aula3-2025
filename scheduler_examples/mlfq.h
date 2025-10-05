#ifndef MLFQ_H
#define MLFQ_H

#include "queue.h"

#define LEVELS 3

void mlfq_scheduler(uint32_t current_time_ms, queue_t *rq[LEVELS], pcb_t **cpu_task);

#endif //MLFQ_H