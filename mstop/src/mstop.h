#ifndef MSTOP_H
#define MSTOP_H

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdatomic.h>

typedef struct {
    uint32_t hr;
    uint32_t min;
    uint32_t sec;
    uint32_t cs;
} time_format;

typedef struct {
    bool running: 1;
    bool paused : 1;
    bool stopped: 1;
    pthread_mutex_t mutex;

    /*atomic_bool running;*/
    /*atomic_bool paused; */
    /*atomic_bool stopped;*/
} program_state;

void* print_stopwatch(void* p_state);

#endif // MSTOP_H
