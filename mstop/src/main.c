#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <termios.h>
#include <signal.h>

#include "handlers.h"
#include "mstop.h"

struct termios old_term_state = {};


// from https://stackoverflow.com/questions/26423537/how-to-position-the-input-text-cursor-in-c
// but only works for unix terminals (xterm, gnome-terminal, ...)???
#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))

// or maybe use curses, but this i think it is overkill for this project
// https://www.gnu.org/software/guile-ncurses/manual/html_node/The-basic-curses-library.html


int main(void) {
    struct termios new_term_state = {}; 

    // save terminal state
    if (tcgetattr(STDIN_FILENO, &old_term_state)) {
        perror("Error: Failed to save terminal state!\n");
        exit(1);
    }

    // register exit handler
    atexit(restore_terminal);
    // register SIGINT handler
    signal(SIGINT, handle_sigint);

    new_term_state = old_term_state;
    // disable canonical mode and echo
    new_term_state.c_lflag &= ~(ICANON | ECHO);
    // apply new state
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_term_state) == -1) {
        perror("Error: Failed to apply new terminal state!\n");
        exit(1);
    }

    program_state state = {
        .running = true,
        .paused  = false
    };

    pthread_t th_input = 0;
    if (pthread_create(&th_input, NULL, handle_input, (void*)&state) != 0) {
        perror("Error: Failed to create thread!\n");
        exit(1);
    }

    // mstop start/stop/pause/reset (this has to check if mstop process is already running) -n [num] (to start another mstop process)
    // p - start/pause
    // s/space - stop
    // r - reset
    // use thread (pthread)
    uint32_t seconds_counter = 0;

    while (state.running) {
        const watch w = calc_hms(seconds_counter);

        if (state.reset) {
            seconds_counter = 0;
            state.reset = false;
        }
        else if (state.paused) {
            continue;
        }

        printf("\r%02u:%02u:%02u", w.hr, w.min, w.sec);
        fflush(stdout);

        sleep(1);
        ++seconds_counter;
    }

    pthread_join(th_input, NULL);
    printf("\n");
    return 0;
}
