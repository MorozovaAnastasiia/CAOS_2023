#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "ieee754_clf.h"

float_class_t classify(double x) {
    // (-1)^s * m * 2^e
    uint64_t notdouble;
    memcpy(&notdouble, &x, sizeof(x));
    uint64_t sign = notdouble >> 63; 
    uint64_t exp = (notdouble >> 52) & 0b11111111111;
    // norm denorm special
    // exp defines
    // norm e != 0 && e != 1111....1111 m = 1...
    // denorm e = 0 m = 0... (around 0)
    // sp 
    // +-inf e = 2^|e| - 1 m = 0 при дел на +-0
    // nan m != 0
    uint64_t fr = notdouble & 0b1111111111111111111111111111111111111111111111111111;
    if (exp == 0b11111111111) {
        if (fr == 0) {
            if (sign == 0) {
                return Inf;
            }
            return MinusInf;
        }
        return NaN;
    }
    if (fr == 0) {
        if (sign == 0) {
            return Zero;
        }
        return MinusZero;
    }
    if (exp == 0) {
        if (sign == 0) {
            return Denormal;
        }
        return MinusDenormal;
    }
    if (sign == 0) {
        return Regular;
    }
    return MinusRegular;
}

