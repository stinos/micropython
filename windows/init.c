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

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#ifdef __INTIME__
#include <mpconfigport.h>
#include <stdio.h>
#include <iwin32.h>
#include <rt.h>
#endif

extern void getbss();

HANDLE hSleepEvent = NULL;

void init() {
    getbss();
    hSleepEvent = CreateEvent(NULL, TRUE, FALSE, FALSE);
#if defined( __MINGW32__ )
    putenv("PRINTF_EXPONENT_DIGITS=2");
#elif !defined( __INTIME__ )
    _set_output_format(_TWO_DIGIT_EXPONENT);
#endif
#ifdef __INTIME__
    //INTime env vars can only set through registry (or some tool maybe),
    //so for now set the main py path to the dir where the executable is
    RTHANDLE mod = GetRtModuleHandle(MICROPY_PORT_COREMODULE);
    char fullPath[MAX_PATH];
    GetRtModuleFilename(mod, fullPath, sizeof(fullPath));
    const size_t len = strlen(fullPath);
    for (size_t i = len; i > 0; --i) {
        if (fullPath[i] == '\\') {
            fullPath[i] = 0;
            break;
        }
        fullPath[i] = 0;
    }
    char env[MAX_PATH];
    sprintf(env, "%s = %s", "MICROPYPATH", fullPath);
    putenv(env);
#endif
}

void deinit() {
    if (hSleepEvent != NULL) {
        CloseHandle(hSleepEvent);
    }
}
