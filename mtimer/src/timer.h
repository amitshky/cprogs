#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <wchar.h>

typedef struct {
    uint32_t h;
    uint32_t m;
    uint32_t s;

    uint32_t time;  // time in seconds
    uint32_t total; // store original (total) time in seconds for progress bar

    wchar_t* progress_bar;
    uint32_t bar_len; // length of the progress bar
} timer;

float elapsed_percent(const timer t);
void generate_progress_bar(const timer t, const float percent);
void print_timer(const timer t);
void calc_hms(timer* const t);

#endif // TIMER_H

