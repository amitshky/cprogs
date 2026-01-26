#include "mstop.h"

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

void calc_hms(time_format* const tf) {
    ++tf->cs;
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

void* print_stopwatch(void* p_data) {
    thread_data* const data = (thread_data*)p_data;

    pthread_mutex_lock(&data->mutex);
    print_time_format(data->tf);
    pthread_mutex_unlock(&data->mutex);

    while (true) {
        pthread_mutex_lock(&data->mutex);

        if (!data->state.running) {
            pthread_mutex_unlock(&data->mutex);
            break;
        }

        if (data->state.stopped) {
            // reset time
            data->tf = (time_format){};
            print_time_format(data->tf);

            // thread wait
            pthread_cond_wait(&data->cond_stop, &data->mutex);
            pthread_mutex_unlock(&data->mutex);
            continue;
        }

        // pthread_cond_timedwait takes absolute time
        // i.e., block thread until a certain time (timestamp)
        struct timespec tspec = {};
        clock_gettime(CLOCK_MONOTONIC, &tspec);
        tspec.tv_nsec += 1e7L; // wake up 1cs from now
        if (tspec.tv_nsec >= 1e9L) {
            tspec.tv_sec += 1L;
            tspec.tv_nsec -= 1e9L;
        }

        // pthread_cond_wait and pthread_cond_timedwait expects the 
        // mutex to be locked, and they unlock the mutex after 
        // suspending the thread, and lock the mutex before
        // unsuspending the thread, so we have to manually unlock
        // the mutex again
        // 
        // suspend thread for 1sec (or 1cs)
        // this can be interrupted by another thread
        pthread_cond_timedwait(&data->cond_sleep, &data->mutex, &tspec);

        if (data->state.paused) {
            // thread wait
            pthread_cond_wait(&data->cond_pause, &data->mutex);
            pthread_mutex_unlock(&data->mutex);
            continue;
        }

        calc_hms(&data->tf);
        print_time_format(data->tf);

        pthread_mutex_unlock(&data->mutex);
    }

    return NULL;
}
