#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <termios.h>
#include <signal.h>

#include "handlers.h"
#include "mstop.h"

// from https://stackoverflow.com/questions/26423537/how-to-position-the-input-text-cursor-in-c
// but only works for unix terminals (xterm, gnome-terminal, ...)???
#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))

// or maybe use curses, but this i think it is overkill for this project
// https://www.gnu.org/software/guile-ncurses/manual/html_node/The-basic-curses-library.html

#define TH_CHECK(x) \
    if (x != 0) {\
        perror("Error: Failed to create thread!\n");\
        exit(1);\
    }
 
#define TC_CHECK(x)\
    if (x == -1) {\
        perror("Error: Failed to access terminal state!\n");\
        exit(1);\
    }

struct termios old_term_state = {};

int main(void) {
    struct termios new_term_state = {}; 

    // save terminal state
    TC_CHECK(tcgetattr(STDIN_FILENO, &old_term_state))
    new_term_state = old_term_state;
    // disable canonical mode and echo
    new_term_state.c_lflag &= ~(ICANON | ECHO);
    // apply new state
    TC_CHECK(tcsetattr(STDIN_FILENO, TCSANOW, &new_term_state))

    // register exit handler
    atexit(restore_terminal);
    // register SIGINT handler
    signal(SIGINT, handle_sigint);

    program_state state = {
        .running = true,
        .paused  = false,
        .stopped = true,
    };

    time_format tf = {};

    thread_data data = {
        .state      = state,
        .tf         = tf,
        .mutex      = PTHREAD_MUTEX_INITIALIZER,
        .cond_pause = PTHREAD_COND_INITIALIZER,
        .cond_stop  = PTHREAD_COND_INITIALIZER,
        .cond_sleep = PTHREAD_COND_INITIALIZER,
    };
    
    pthread_mutex_init(&data.mutex, NULL);
    pthread_cond_init(&data.cond_pause, NULL);
    pthread_cond_init(&data.cond_stop, NULL);

    // use MONOTONIC clock
    pthread_condattr_t attr;
    pthread_condattr_init(&attr);
    pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    pthread_cond_init(&data.cond_sleep, &attr);
    pthread_condattr_destroy(&attr);

    pthread_t th_stopw = {};
    pthread_t th_input = {};

    TH_CHECK(pthread_create(&th_stopw, NULL, print_stopwatch, (void*)&data))
    TH_CHECK(pthread_create(&th_input, NULL, handle_input,    (void*)&data))

    pthread_join(th_stopw, NULL);
    pthread_join(th_input, NULL);

    pthread_cond_destroy(&data.cond_sleep);
    pthread_cond_destroy(&data.cond_stop);
    pthread_cond_destroy(&data.cond_pause);
    pthread_mutex_destroy(&data.mutex);

    printf("\n");
    return 0;
}
