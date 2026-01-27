#include "timer.h"

#include "math.h"
#include "stdio.h"

float elapsed_percent(const timer t) {
    if (t.total == 0)
        return 100.0f;

    return 100.0f * (1.0f - (float)t.time / t.total);
}

void generate_progress_bar(const timer t, const float percent) {
    uint32_t iter = t.bar_len * (percent / 100.0f);

    for (uint32_t i = 0; i < iter; ++i) {
        t.progress_bar[i] = L'█';
    }

    for (uint32_t i = iter; i < t.bar_len; ++i) {
        t.progress_bar[i] = L'▒';
    }

    t.progress_bar[t.bar_len] = '\0';
}

void print_timer(const timer t) {
    const float percent = elapsed_percent(t);
    generate_progress_bar(t, percent);

    // %% is needed to print '%'
    wprintf(L"\r%3uh %02um %02us %ls %3d%%", t.h, t.m, t.s, t.progress_bar,
            (int)floor(percent));

    // so that the timer is printed immediately
    fflush(stdout);
}

void calc_hms(timer* const t) {
    const float f_hr = t->time / 3600.0f;
    t->h = (uint32_t)f_hr;

    const float f_min = (f_hr - t->h) * 60.0f;
    t->m = (uint32_t)f_min;

    t->s = (uint32_t)roundf((f_min - t->m) * 60.0f);

    // to avoid cases like this
    // 3h 37m 01s to 3h 36m 60s
    // this happens when rounding off values like 0.99999
    // eg: if t.time = 13020 seconds then f_hr = 3.616667 and f_min = 36.999992
    //     which means t.s = 60 (because of roundf())
    if (t->s == 60) {
        t->s = 0;
        ++t->m;
    }

    /*wprintf(L"  [(%5us) h: %u (%f), m: %u (%f), s: %u]", t->time, t->h, f_hr,
   * t->m, f_min, t->s);*/
}
