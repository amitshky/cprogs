#ifndef MSTOP_H

#include <stdint.h>

typedef struct {
    uint32_t hr;
    uint32_t min;
    uint32_t sec;
    uint32_t ds;
    uint32_t ms;
} time_format;

typedef struct {
    uint8_t running: 1; // is program running
    uint8_t paused : 1; // is stopwatch paused
    uint8_t stopped: 1; // start/stop stopwatch
} program_state;

void calc_hms(time_format* tf);

void* print_stopwatch(void* p_state);

#endif // MSTOP_H
