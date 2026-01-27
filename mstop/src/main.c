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

inline void th_check(int errcode) {
    if (errcode != 0) {
        perror("Error: Failed to create thread!\n");
        exit(1);
    }
}
 
inline void tc_check(int errcode) {
    if (errcode == -1) {
        perror("Error: Failed to access terminal state!\n");
        exit(1);
    }
}

void print_usage();
void handle_sigint();
void restore_terminal();

int main(int argc, char** argv) {
    thread_data data = {
        .state = {
            .quit    = false,
            .paused  = false,
            .running = false,
        },
        .watch = {},
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .cond  = PTHREAD_COND_INITIALIZER,
    };

    // by default, the terminal is set to run in non-canonical mode without echo
    bool normal_mode = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage();
            exit(0);
        }
        else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--start") == 0) {
            data.state.running = true;
            clock_gettime(CLOCK_MONOTONIC, &data.watch.start_time);
        }
        else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--normal") == 0) {
            normal_mode = true;
        }
        else {
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
        signal(SIGINT, handle_sigint);
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

void print_usage() {
    printf("USAGE:\n");
    printf("    mstop [OPTIONS]\n\n");
    printf("OPTIONS:\n");
    printf("    -h, --help   : Show this help message and exit.\n");
    printf("    -s, --start  : Start the stopwatch immediately.\n");
    printf("    -n, --normal : Run the terminal in normal mode (canonical input with\n");
    printf("                   echo). By default, the terminal is configured in\n");
    printf("                   non-canonical mode without echo. Enabling this option\n");
    printf("                   requires pressing ENTER after each key input. Use this\n");
    printf("                   option if your terminal is non-UNIX or does not support\n");
    printf("                   canonical mode.\n\n");
    printf("KEY INPUTS:\n");
    printf("    s            : Start or reset the stopwatch.\n");
    printf("    p, <space>   : Pause or resume the stopwatch.\n");
    printf("    q            : Quit the program.\n");
}

// handle SIGINT (Ctrl+C)
void handle_sigint() {
    restore_terminal();
    printf("\n");
    exit(0);
}

void restore_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_term_state);
}
