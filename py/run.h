#ifndef MICROPYTHON_RUN_H
#define MICROPYTHON_RUN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "./obj.h"
#include <stdint.h>

void mp_run_init(char *heap, size_t heap_size, char *stack, size_t stack_size);

int mp_run_execfile(const char *file);

int mp_run_parse(const char *string, int is_repl, mp_obj_t *compiled_object);

int mp_run_execfun(mp_obj_t compiled_object, mp_obj_t return_value);

int mp_run_execstring(const char *string, int is_repl);

int mp_run_evalstring(const char *string, mp_obj_t *return_value);

void mp_run_deinit();

#ifdef __cplusplus
}
#endif

#endif // #ifndef MICROPYTHON-MICROPYTHON_RUN_H
