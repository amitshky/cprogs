#include "handlers.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "mstop.h"

extern struct termios old_term_state;
extern pthread_mutex_t state_mutex;

void* handle_input(void* p_state) {
    program_state* const state = (program_state*)p_state;
    char ch = {};

    while (read(STDIN_FILENO, &ch, 1) > 0) {
        pthread_mutex_lock(&state->mutex);

        // quit
        if (ch == 'q') {
            state->running = false;
            pthread_mutex_unlock(&state->mutex);
            break;
        }
        // start/stop
        else if (ch == 's') {
            state->stopped = !state->stopped;
            state->paused = false;
        }
        // pause/resume
        else if ((ch == 'p' || ch == ' ') && !state->stopped) {
            state->paused = !state->paused;
        }

        pthread_mutex_unlock(&state->mutex);
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
