/*
 * Implementation of a queue data structure, used for buffering data for
 * serial communication (UART)
 *
 */

#include "plibi_queue.h"


#ifdef PL_QUEUE_STATISTICS
int pli_queue_statistics_free(pli_queue* q){
    int free=q->read - q->write - 1;
    if (free < 0) free += q->size;
    return free;
}

void pli_queue_statistics_reset(pli_queue* q){
    q->min=q->size-1;
}
#endif

void pli_queue_init(pli_queue* q, uint8_t* buffer, uint_fast16_t size)
{
    q->read = q->write = 0;
    q->size = size;
    q->buffer = buffer;
#ifdef PL_QUEUE_STATISTICS
    pli_queue_statistics_reset(q);
#endif
}

int pli_queue_full(pli_queue* q)
{
    return (((q->write + 1) % q->size) == q->read);
}

int pli_queue_empty(pli_queue* q)
{
    return (q->write == q->read);
}

int pli_enqueue(pli_queue* q, uint8_t data)
{
    if (pli_queue_full(q))
        return 0;
    else {
        q->buffer[q->write] = data;
        q->write = ((q->write + 1) == q->size) ? 0 : q->write + 1;
#ifdef PL_QUEUE_STATISTICS
        uint_fast16_t min=pli_queue_statistics_free(q);
        if (min < q->min) q->min=min;
#endif

    }
    return 1;
}

int pli_dequeue(pli_queue* q, uint8_t* data)
{
    if (pli_queue_empty(q))
        return 0;
    else {
        *data = q->buffer[q->read];
        q->read = ((q->read + 1) == q->size) ? 0 : q->read + 1;
    }
    return 1;
}
