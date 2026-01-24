#include "mstop.h"

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

void calc_hms(time_format* const tf) {
    if (tf->cs < 100)
        return;
    tf->cs = 0;

    ++tf->sec;
    if (tf->sec < 60)
        return;
    tf->sec = 0;

    ++tf->min;
    if (tf->min < 60)
        return;
    tf->min = 0;

    ++tf->hr;
}

void print_time_format(const time_format tf) {
    printf("\r%02lu:%02lu:%02lu.%02lu", tf.hr, tf.min, tf.sec, tf.cs);
    fflush(stdout);
}

// get unix timestamp in centiseconds
uint64_t get_csec(const struct timeval ts) {
    return (uint64_t)(ts.tv_sec * 100) + (uint64_t)(ts.tv_usec * 1e-4f);
}

void* print_stopwatch(void* p_state) {
    program_state* const state = (program_state*)p_state;

    time_format tf = {};
    print_time_format(tf);

    // using unix timestamp to get more accurate timings
    // becuase the stopwatch was lagging behind when i 
    // only used nanosleep()
    struct timeval tval = {
        .tv_usec = 1,
    };
    gettimeofday(&tval, NULL);
    uint64_t timestamp_cs = get_csec(tval);

    while (true) {
        pthread_mutex_lock(&state->mutex);
        bool running = state->running;
        bool paused = state->paused;
        bool stopped = state->stopped;
        pthread_mutex_unlock(&state->mutex);

        if (!running) {
            break;
        }

        if (stopped) {
            tf = (time_format){};
            print_time_format(tf);
            continue;
        }

        if (paused) {
            continue;
        }

        // get the accurate duration by calculating how much time 
        // has actually passed
        gettimeofday(&tval, NULL);
        const uint64_t timestamp_cs2 = get_csec(tval);
        const uint64_t tick = timestamp_cs2 - timestamp_cs;
        timestamp_cs = timestamp_cs2;

        tf.cs += tick;
        calc_hms(&tf);

        // the stopwatch could be implemented without sleep,
        // but sleep will suspend this thread becuase, the thread
        // doesn't need to be running all the time
        struct timespec tspec = {
            .tv_nsec = 1e7, // 10^7ns = 1cs
        };
        nanosleep(&tspec, NULL);

        print_time_format(tf);
    }

    return NULL;
}
