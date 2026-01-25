#include "handlers.h"

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <termios.h>

#include "mstop.h"

extern struct termios old_term_state;

void* handle_input(void* p_data) {
    thread_data* const data = (thread_data*)p_data;
    char ch = {};

    while (read(STDIN_FILENO, &ch, 1) > 0) {
        pthread_mutex_lock(&data->mutex);

        // quit
        if (ch == 'q') {
            data->state.running = false;

            pthread_cond_signal(&data->cond_sleep);
            pthread_cond_signal(&data->cond_stop);
            pthread_cond_signal(&data->cond_pause);
            pthread_mutex_unlock(&data->mutex);
            break;
        }
        // start/stop
        else if (ch == 's') {
            data->state.stopped = !data->state.stopped;

            // to be able to stop if paused
            data->state.paused = false;
            // if the thread is sleeping, signal it first,
            // same if it is suspended by pause condition
            pthread_cond_signal(&data->cond_sleep);
            pthread_cond_signal(&data->cond_pause);

            if (!data->state.stopped) {
                pthread_cond_signal(&data->cond_stop);
            }
        }
        // pause/resume
        else if ((ch == 'p' || ch == ' ') && !data->state.stopped) {
            data->state.paused = !data->state.paused;

            if (!data->state.paused) {
                pthread_cond_signal(&data->cond_sleep);
                pthread_cond_signal(&data->cond_pause);
            }
        }

        pthread_mutex_unlock(&data->mutex);
    }

    return NULL;
}

// handle SIGINT (Ctrl+C)
void handle_sigint() {
    restore_terminal();
    exit(0);
}

void restore_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_term_state);
}
