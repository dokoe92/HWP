
#ifndef PLIBI_SERIAL_H_
#define PLIBI_SERIAL_H_

#include <stdint.h>
#include "plibi_board.h"
#include "plibi_queue.h"

int pli_serial_init(uint32_t baud);

void pli_serial_write(uint8_t data);

int pli_serial_read(uint8_t* data);

#ifdef PL_QUEUE_STATISTICS
void pli_serial_statistics_read(int* read, int* write);
#endif

#endif
