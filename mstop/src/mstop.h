#ifndef MSTOP_H

#include <stdint.h>

typedef struct {
    uint32_t hr;
    uint32_t min;
    uint32_t sec;
} watch;

typedef struct {
    uint8_t running: 1;
    uint8_t paused : 1;
    uint8_t reset  : 1;
} program_state;

watch calc_hms(const uint32_t sec);

#endif // MSTOP_H
