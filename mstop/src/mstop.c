#include "mstop.h"

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

void calc_hms(time_format* tf) {
    /*++tf->ms;*/
    /**/
    /*if (tf->ms != 1000)*/
    /*    return;*/

    /*tf->ms = 0;*/
    /*++tf->ds;*/
    /**/
    /*if (tf->ds != 100)*/
    /*    return;*/

    tf->ds = 0;
    ++tf->sec;

    if (tf->sec != 60)
        return;

    tf->sec = 0;
    ++tf->min;

    if (tf->min != 60)
        return;

    ++tf->hr;
    tf->min = 0;
}

void print_time_format(const time_format tf) {
    printf("\r%02u:%02u:%02u.%02u", tf.hr, tf.min, tf.sec, tf.ds);
    fflush(stdout);
}

void* print_stopwatch(void* p_state) {
    program_state* state = (program_state*)p_state;

    time_format tf = {};
    print_time_format(tf);

    while (state->running) {
        if (state->stopped) {
            tf = (time_format){};
            print_time_format(tf);
            continue;
        }

        if (state->paused) {
            continue;
        }

        // if the stopwatch was stopped when this thread
        // was asleep, don't print the time format
        if (state->running) {
            calc_hms(&tf);
        }

        sleep(1);

        // printing this after sleep because
        // there is a print before the while loop
        if (state->running) {
            print_time_format(tf);
        }
    }

    pthread_exit(NULL);
    return NULL;
}
