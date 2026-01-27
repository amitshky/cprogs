#ifndef MSTOP_H
#define MSTOP_H

#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

typedef struct {
    uint64_t hr;
    uint64_t min;
    uint64_t sec;
    uint64_t ms;

    uint64_t elapsed_ms;
    struct timespec start_time;
} stopwatch;

typedef struct {
    bool quit   : 1;
    bool paused : 1;
    bool running: 1;
} program_state;

typedef struct {
    program_state state;
    stopwatch watch;

    pthread_mutex_t mutex;
    pthread_cond_t cond;
} thread_data;

void stopwatch_quit(thread_data* const data);
void stopwatch_start(thread_data* const data);
void stopwatch_pause(thread_data* const data);
void* stopwatch_print(void* p_state);

#endif // MSTOP_H
