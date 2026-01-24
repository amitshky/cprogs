#include "mstop.h"

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

void calc_hms(time_format* const tf) {
    /*++tf->ms;*/
    /*if (tf->ms != 1000)*/
    /*    return;*/
    /*tf->ms = 0;*/

    /*++tf->ds;*/
    /*if (tf->ds != 100)*/
    /*    return;*/
    /*tf->ds = 0;*/

    ++tf->sec;
    if (tf->sec != 60)
        return;
    tf->sec = 0;

    ++tf->min;
    if (tf->min != 60)
        return;
    tf->min = 0;

    ++tf->hr;
}

void print_time_format(const time_format tf) {
    printf("\r%02u:%02u:%02u.%02u", tf.hr, tf.min, tf.sec, tf.ds);
    fflush(stdout);
}

void* print_stopwatch(void* p_state) {
    program_state* const state = (program_state*)p_state;

    time_format tf = {};
    print_time_format(tf);

    while (true) {
        pthread_mutex_lock(&state->mutex);
        bool running = state->running;
        bool paused = state->paused;
        bool stopped = state->stopped;
        pthread_mutex_unlock(&state->mutex);

        if (!running)
            break;

        if (stopped) {
            tf = (time_format){};
            print_time_format(tf);
            continue;
        }

        if (paused) {
            continue;
        }

        calc_hms(&tf);
        sleep(1);
        print_time_format(tf);
    }

    return NULL;
}
