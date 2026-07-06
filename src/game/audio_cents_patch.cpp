#include <cstdint>

#include "recomp.h"

extern "C" void alCents2Ratio(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram;

    int32_t cents = static_cast<int32_t>(ctx->r4);
    float x;
    float ratio = 1.0f;

    if (cents >= 0) {
        x = 1.00057779f;
    } else {
        x = 0.9994225441f;
        cents = -cents;
    }

    while (cents) {
        if (cents & 1) {
            ratio *= x;
        }
        x *= x;
        cents >>= 1;
    }

    ctx->f0.fl = ratio;
}
