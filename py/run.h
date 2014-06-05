#ifndef MICROPYTHON_RUN_H
#define MICROPYTHON_RUN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

void mp_run_init(char *heap, size_t heap_size);

int mp_run_execfile(const char *file);

void *mp_run_parse(const char *string, int asRepl);

int mp_run_execfun(void *parsedFun);

int mp_run_execstring(const char *string, int asRepl);

void mp_run_deinit();

#ifdef __cplusplus
}
#endif

#endif //#ifndef MICROPYTHON-MICROPYTHON_RUN_H
