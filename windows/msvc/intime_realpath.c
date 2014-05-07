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
#include <errno.h>
#include <Windows.h>

int access(const char *path, int n) {
    if (n == 4) {
        HANDLE h = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
        if (h != INVALID_HANDLE_VALUE && h != 0) {
            CloseHandle(h);
            return 0;
        }
    }
    errno = ENOENT;
    return -1;
}

char* _fullpath(char *absPath, const char *relPath, size_t maxLength) {
    if (absPath == NULL)
        absPath = (char*)malloc(maxLength);
    GetFullRtPathName(relPath, maxLength, absPath, NULL);
    return absPath;
}
