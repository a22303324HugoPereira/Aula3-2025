#define LEVELS 3
#define QUANTUM_MS 500

void mlfq_scheduler(uint32_t current_time_ms, queue_t *rq[LEVELS], pcb_t **cpu_task) {
    if (*cpu_task) {
        (*cpu_task)->ellapsed_time_ms += TICKS_MS;
        (*cpu_task)->quantum_left -= TICKS_MS;

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
        else if ((*cpu_task)->quantum_left <= 0) {
            int lvl = (*cpu_task)->priority;
            if (lvl < LEVELS-1) lvl++;
            (*cpu_task)->priority = lvl;
            (*cpu_task)->quantum_left = QUANTUM_MS;
            enqueue_pcb(&rq[lvl], *cpu_task);
            *cpu_task = NULL;
        }
    }

    if (*cpu_task == NULL) {
        for (int i=0; i<LEVELS; i++) {
            *cpu_task = dequeue_pcb(&rq[i]);
            if (*cpu_task) {
                (*cpu_task)->quantum_left = QUANTUM_MS;
                break;
            }
        }
    }
}