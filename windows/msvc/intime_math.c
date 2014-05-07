/*
* This file is part of the Micro Python project, http://micropython.org/
*
* The MIT License (MIT)
*
* Copyright (c) 2013, 2014 Damien P. George
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include <math.h>
#include <errno.h>

double log2(double n) {
    return log(n) / log(2);
}

// from https://github.com/wayneeseguin/miniruby/blob/master/missing/tgamma.c
#define PI      3.14159265358979324
#define LOG_2PI 1.83787706640934548
#define N       8

#define B0  1
#define B1  (-1.0 / 2.0)
#define B2  ( 1.0 / 6.0)
#define B4  (-1.0 / 30.0)
#define B6  ( 1.0 / 42.0)
#define B8  (-1.0 / 30.0)
#define B10 ( 5.0 / 66.0)
#define B12 (-691.0 / 2730.0)
#define B14 ( 7.0 / 6.0)
#define B16 (-3617.0 / 510.0)

double loggamma(double x) {
    double v, w;
    v = 1;
    while (x < N) { v *= x;  x++; }
    w = 1 / (x * x);
    return ((((((((B16 / (16 * 15))  * w + (B14 / (14 * 13))) * w
        + (B12 / (12 * 11))) * w + (B10 / (10 * 9))) * w
        + (B8 / (8 * 7))) * w + (B6 / (6 * 5))) * w
        + (B4 / (4 * 3))) * w + (B2 / (2 * 1))) / x
        + 0.5 * LOG_2PI - log(v) - x + (x - 0.5) * log(x);
}

double tgamma(double x) {
    if (x == 0.0) {
        errno = ERANGE;
        return 1 / x < 0 ? -HUGE_VAL : HUGE_VAL;
    }
    if (x < 0) {
        int sign;
        static double zero = 0.0;
        double i, f;
        f = modf(-x, &i);
        if (f == 0.0) {
            errno = EDOM;
            return zero / zero;
        }
        sign = (fmod(i, 2.0) != 0.0) ? 1 : -1;
        return sign * PI / (sin(PI * f) * exp(loggamma(1 - x)));
    }
    return exp(loggamma(x));
}
