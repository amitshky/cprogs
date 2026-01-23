#include "handlers.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "mstop.h"

extern struct termios old_term_state;

void* handle_input(void* p_state) {
    program_state* state = (program_state*)p_state;
    char ch = '\0';

    while (true) {
        // read 1 character
        if (read(STDIN_FILENO, &ch, 1) == -1) {
            perror("Error: Failed to read input!\n");
            exit(1);
        }

        // quit
        if (ch == 'q') {
            state->running = false;
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
    }

    pthread_exit(NULL);
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
