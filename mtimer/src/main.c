#include <stdio.h>
#include <string.h>
#include <stdint.h> // for uint32_t, etc
#include <stdlib.h> // for exit(), free(), malloc()
#include <unistd.h> // for sleep()
#include <wchar.h>
#include <locale.h> // for setlocale()

#include "timer.h"

void print_usage();

int main(int argc, char** argv) {
    setlocale(LC_ALL, ""); // to be able to print the progress bar

    timer t = {
        .bar_len = 50
    };

    if (argc <= 1) {
        fprintf(stderr, "ERROR: No arguments provided.\n");
        print_usage();
        exit(1);
    }

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        print_usage();
        exit(0);
    }

    // parse args
    for (int i = 1; i < argc; ++i) {
        // to store the number part of the arg (eg: store 45 from 45h)
        char num[33] = {};
        for (int j = 0; argv[i][j] != '\0' ; ++j) {
            if (j > 32) {
                fprintf(stderr,
                    "ERROR: Argument %d is too long. [Max length: 32]\n", i);
                exit(1);
            }

            // check if the character is not a number
            if ((int)argv[i][j] < (int)'0' || (int)argv[i][j] > (int)'9') {
                num[j] = '\0';

                if (argv[i][j] == 'h') {
                    t.h = atoi(num);
                }
                else if (argv[i][j] == 'm') {
                    t.m = atoi(num);
                }
                else if (argv[i][j] == 's') {
                    t.s = atoi(num);
                }

                break;
            }

            // only store the number
            num[j] = argv[i][j];
        }
    }

    if (t.h == 0 && t.m == 0 && t.s == 0) {
        fprintf(stderr, "ERROR: Invalid Argument(s).\n");
        print_usage();
        exit(1);
    }

    t.time = t.h * 3600 + t.m * 60 + t.s;
    t.total = t.time;
    t.progress_bar = (wchar_t*)malloc((t.bar_len + 1) * sizeof(wchar_t));

    while (t.time) {
        calc_hms(&t);
        print_timer(t);
        --t.time;

        sleep(1);
    }

    calc_hms(&t);
    print_timer(t);
    wprintf(L"\n");

    free((void*)t.progress_bar);

    return 0;
}

void print_usage() {
    printf("Usage:\n\ttimer [-h|--help][<num>h][<num>m][<num>s]\n");
}
