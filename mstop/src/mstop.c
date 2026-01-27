#define _POSIX_C_SOURCE 200809L

#include "mstop.h"

#include <stdio.h>
#include <sys/time.h>
#include <time.h>

void calc_hms(uint64_t ms, stopwatch* const w) {
    w->hr  = ms / 3600000;
    ms %= 3600000;
    w->min = ms / 60000;
    ms %= 60000;
    w->sec = ms / 1000;
    w->ms  = ms % 1000;
}

void print_time(const stopwatch w) {
    printf("\r%02lu:%02lu:%02lu.%03lu", w.hr, w.min, w.sec, w.ms);
    fflush(stdout);
}

uint64_t duration_ms(const struct timespec start, const struct timespec end) {
    return (uint64_t)((end.tv_sec - start.tv_sec) * 1000
            + (end.tv_nsec - start.tv_nsec) * 1e-6);
}

void stopwatch_quit(thread_data* const data) {
    pthread_mutex_lock(&data->mutex);
    data->state.quit = true;
    pthread_cond_signal(&data->cond);
    pthread_mutex_unlock(&data->mutex);
}

void stopwatch_start(thread_data* const data) {
    pthread_mutex_lock(&data->mutex);

    data->state.start = !data->state.start;
    data->watch = (stopwatch){};

    if (data->state.start) {
        clock_gettime(CLOCK_MONOTONIC, &data->watch.start_time);
        data->state.pause = false;
        pthread_cond_signal(&data->cond);
    }

    pthread_mutex_unlock(&data->mutex);
}

void stopwatch_pause(thread_data* const data) {
    pthread_mutex_lock(&data->mutex);

    // resume
    if (data->state.pause) {
        data->state.pause = false;
        clock_gettime(CLOCK_MONOTONIC, &data->watch.start_time);
        pthread_cond_signal(&data->cond);
    }
    // pause
    else {
        data->state.pause = true;
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        data->watch.elapsed_ms += duration_ms(data->watch.start_time, now);
    }

    pthread_mutex_unlock(&data->mutex);
}

void* stopwatch_print(void* p_data) {
    thread_data* const data = (thread_data*)p_data;
    struct timespec now;

    while (true) {
        pthread_mutex_lock(&data->mutex);

        if (data->state.quit) {
            pthread_mutex_unlock(&data->mutex);
            break;
        }

        if (!data->state.start || data->state.pause) {
            print_time(data->watch);
            pthread_cond_wait(&data->cond, &data->mutex);
            pthread_mutex_unlock(&data->mutex);
            continue;
        }

        clock_gettime(CLOCK_MONOTONIC, &now);
        uint64_t displayed_ms = 
            duration_ms(data->watch.start_time, now) + data->watch.elapsed_ms;

        calc_hms(displayed_ms, &data->watch);
        print_time(data->watch);

        pthread_mutex_unlock(&data->mutex);

        // sleep for 1ms
        struct timespec t = { 0, 1e6 };
        nanosleep(&t, NULL);
    }

    printf("\n");
    return NULL;
}
