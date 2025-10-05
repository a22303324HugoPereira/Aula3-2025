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

    if (*cpu_task == NULL) {
        pcb_t *shortest = NULL, *prev = NULL, *it = rq->head, *it_prev = NULL;
        while (it) {
            if (!shortest || it->time_ms < shortest->time_ms) {
                shortest = it;
                prev = it_prev;
            }
            it_prev = it;
            it = it->next;
        }
        if (shortest) {
            if (prev) prev->next = shortest->next;
            else rq->head = shortest->next;
            if (rq->tail == shortest) rq->tail = prev;
            shortest->next = NULL;

            *cpu_task = shortest;
        }
    }
}