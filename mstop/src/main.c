#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>

typedef struct {
    uint32_t hr;
    uint32_t min;
    uint32_t sec;
} watch;

watch calc_hms(uint32_t sec) {
    watch w = {};
    const float f_hr = sec / 3600.0f;
    w.hr = (uint32_t)f_hr;

    const float f_min = (f_hr - w.hr) * 60.0f;
    w.min = (uint32_t)f_min;

    w.sec = (uint32_t)roundf((f_min - w.min) * 60.0f);

    // to avoid cases like this
    // 3h 37m 01s to 3h 36m 60s
    // this happens when rounding off values like 0.99999
    // eg: if t.time = 13020 seconds then f_hr = 3.616667 and f_min = 36.999992
    //     which means t.s = 60
    if (w.sec == 60) {
        w.sec = 0;
        ++w.min;
    }

    return w;
}

int main(void) {
    uint32_t seconds_counter = 0;

    while (seconds_counter < 1000000) {
        const watch w = calc_hms(seconds_counter);

        printf("\r%02u:%02u:%02u", w.hr, w.min, w.sec);
        fflush(stdout);
        ++seconds_counter;

        sleep(1);
    }

    printf("\n");
    return 0;
}
