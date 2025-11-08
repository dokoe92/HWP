#ifndef PLIBI_QUEUE_H_
#define PLIBI_QUEUE_H_

#include <stdint.h>

#include "plib_config.h"

typedef struct pli_queue {
    uint_fast16_t read, write;
    uint_fast16_t size;
    uint8_t* buffer;
#ifdef PL_QUEUE_STATISTICS
    uint_fast16_t min;
#endif
} pli_queue;

#ifdef PL_QUEUE_STATISTICS
int pli_queue_statistics_free(pli_queue* q);

void pli_queue_statistics_reset(pli_queue* q);
#endif

void pli_queue_init(pli_queue* q, uint8_t* buffer, uint_fast16_t size);

int pli_queue_full(pli_queue* q);

int pli_queue_empty(pli_queue* q);

int pli_enqueue(pli_queue* q, uint8_t data);

int pli_dequeue(pli_queue* q, uint8_t* data);

#endif
