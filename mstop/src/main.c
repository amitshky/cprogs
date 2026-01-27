#define _POSIX_C_SOURCE 200809L

#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "mstop.h"

struct termios old_term_state = {};

static inline void th_check(int ret) {
    if (ret != 0) {
        perror("Error: Failed to create thread!\n");
        exit(1);
    }
}

static inline void tc_check(int ret) {
    if (ret == -1) {
        perror("Error: Failed to access terminal state!\n");
        exit(1);
    }
}

static inline void print_usage() {
    printf("USAGE:\n"
           "    mstop [OPTIONS]\n\n"
           "OPTIONS:\n"
           "    -h, --help   : Show this help message and exit.\n"
           "    -s, --start  : Start the stopwatch immediately.\n"
           "    -n, --normal : Run the terminal in normal mode (canonical\n"
           "                   input with echo). By default, the terminal\n"
           "                   is configured in non-canonical mode without\n"
           "                   echo. Enabling this option requires pressing\n"
           "                   ENTER after each key input. Use this option\n"
           "                   if your terminal is non-UNIX or does not\n"
           "                   support canonical mode.\n\n"
           "KEY INPUTS:\n"
           "    s            : Start or reset the stopwatch.\n"
           "    p, <space>   : Pause or resume the stopwatch.\n"
           "    q            : Quit the program.\n");
}

static inline void restore_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_term_state);
}

// handle SIGINT (Ctrl+C)
static inline void handle_sigint(int sig) {
    (void)sig;
    restore_terminal();
    printf("\n");
    exit(0);
}

int main(int argc, char** argv) {
    thread_data data = {
        .state =
            {
                .quit = false,
                .pause = false,
                .start = false,
            },
        .watch = {},
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .cond = PTHREAD_COND_INITIALIZER,
    };

    bool normal_mode = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage();
            exit(0);
        } else if (strcmp(argv[i], "-s") == 0 ||
                   strcmp(argv[i], "--start") == 0) {
            data.state.start = true;
            clock_gettime(CLOCK_MONOTONIC, &data.watch.start_time);
        } else if (strcmp(argv[i], "-n") == 0 ||
                   strcmp(argv[i], "--normal") == 0) {
            normal_mode = true;
        } else {
            fprintf(stderr, "ERROR: Invalid arguments provided!\n");
            print_usage();
            exit(1);
        }
    }

    if (!normal_mode) {
        struct termios new_term_state = {};
        // save terminal state
        tc_check(tcgetattr(STDIN_FILENO, &old_term_state));
        new_term_state = old_term_state;
        // disable canonical mode and echo
        new_term_state.c_lflag &= ~(ICANON | ECHO);
        // apply new state
        tc_check(tcsetattr(STDIN_FILENO, TCSANOW, &new_term_state));
        // register exit handler
        atexit(restore_terminal);
        // register SIGINT handler
        struct sigaction act = {0};
        act.sa_handler = handle_sigint;
        sigemptyset(&act.sa_mask);
        sigaction(SIGINT, &act, NULL);
    }

    pthread_mutex_init(&data.mutex, NULL);
    pthread_cond_init(&data.cond, NULL);

    pthread_t th_stopw = {};
    th_check(pthread_create(&th_stopw, NULL, stopwatch_print, (void*)&data));

    char ch = {};
    while (read(STDIN_FILENO, &ch, 1) > 0) {
        // quit
        if (ch == 'q') {
            stopwatch_quit(&data);
            break;
        }
        // start/reset
        else if (ch == 's') {
            stopwatch_start(&data);
        }
        // pause/resume
        else if ((ch == 'p') || (ch == ' ')) {
            stopwatch_pause(&data);
        }
    }

    pthread_join(th_stopw, NULL);
    pthread_cond_destroy(&data.cond);
    pthread_mutex_destroy(&data.mutex);

    return 0;
}
