#include "mstop.h"

#include "math.h"

watch calc_hms(const uint32_t sec) {
    watch w = {};
    const float f_hr = sec / 3600.0f;
    w.hr = (uint32_t)f_hr;

    const float f_min = (f_hr - w.hr) * 60.0f;
    w.min = (uint32_t)f_min;

    w.sec = (uint32_t)roundf((f_min - w.min) * 60.0f);

    // to avoid cases like this
    // 3h 37m 01s to 3h 36m 60s
    // this happens when rounding off values like 0.99999
    // eg: if sec = 13020 seconds then f_hr = 3.616667 and f_min = 36.999992
    //     which means w.sec = 60 (beause of roundf())
    if (w.sec == 60) {
        w.sec = 0;
        ++w.min;
    }

    return w;
}

