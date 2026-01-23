#include "handlers.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "mstop.h"

extern struct termios old_term_state;

void* handle_input(void* state) {
    char ch = '\0';
    // read 1 character
    while (true) {
        if (read(STDIN_FILENO, &ch, 1) == -1) {
            perror("Error: Failed to read input!\n");
            exit(1);
        }

        if (ch == 'q') {
            ((program_state*)state)->running = false;
            break;
        }
        else if (ch == 'p' || ch == ' ') {
            ((program_state*)state)->paused = !((program_state*)state)->paused;
        }
        else if (ch == 'r') {
            ((program_state*)state)->reset = true;
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
