#ifndef MSTOP_H
#define MSTOP_H

#include <stdint.h>
#include <stdatomic.h>

typedef struct {
    uint32_t hr;
    uint32_t min;
    uint32_t sec;
    uint32_t ds;
    uint32_t ms;
} time_format;

typedef struct {
    atomic_bool running; // is program running
    atomic_bool paused; // is stopwatch paused
    atomic_bool stopped; // is stopwatch stopped
} program_state;

void calc_hms(time_format* const tf);

void* print_stopwatch(void* p_state);

#endif // MSTOP_H
