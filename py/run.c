#include "run.h"
#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "objlist.h"
#include "runtime.h"
#include "gc.h"
#include "lexer.h"
#include "parse.h"
#include "compile.h"
#include "stackctrl.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

extern int execute_from_lexer(int source_kind, const void *source, mp_parse_input_kind_t input_kind, bool is_repl);
extern int handle_uncaught_exception(mp_obj_base_t *exc);

#ifdef _WIN32
#define PATHLIST_SEP_CHAR ';'
#else
#define PATHLIST_SEP_CHAR ':'
#endif

void mp_run_init(char *heap, size_t heap_size){
    mp_stack_ctrl_init();
    mp_stack_set_limit(40000 * (BYTES_PER_WORD / 4));

#if MICROPY_ENABLE_GC
    gc_init(heap, heap + heap_size);
#endif

    mp_init();

    char *home = getenv("HOME");
    char *path = getenv("MICROPYPATH");
    if (path == NULL) {
        path = "~/.micropython/lib:/usr/lib/micropython";
    }
    size_t path_num = 1; // [0] is for current dir (or base dir of the script)
    for (char *p = path; p != NULL; p = strchr(p, PATHLIST_SEP_CHAR)) {
        path_num++;
        if (p != NULL) {
            p++;
        }
    }
    mp_obj_list_init(mp_sys_path, path_num);
    mp_obj_t *path_items;
    mp_obj_list_get(mp_sys_path, &path_num, &path_items);
    path_items[0] = MP_OBJ_NEW_QSTR(MP_QSTR_);
    char *p = path;
    for (size_t i = 1; i < path_num; i++) {
        char *p1 = strchr(p, PATHLIST_SEP_CHAR);
        if (p1 == NULL) {
            p1 = p + strlen(p);
        }
        if (p[0] == '~' && p[1] == '/' && home != NULL) {
            // Expand standalone ~ to $HOME
            CHECKBUF(buf, PATH_MAX);
            CHECKBUF_APPEND(buf, home, strlen(home));
            CHECKBUF_APPEND(buf, p + 1, p1 - p - 1);
            path_items[i] = MP_OBJ_NEW_QSTR(qstr_from_strn(buf, CHECKBUF_LEN(buf)));
        } else {
            path_items[i] = MP_OBJ_NEW_QSTR(qstr_from_strn(p, p1 - p));
        }
        p = p1 + 1;
    }
  
    mp_obj_list_init(mp_sys_argv, 0);
}

void *mp_run_parse(const char *string, int asRepl) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, string, strlen(string), false);
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_SINGLE_INPUT);
        mp_obj_t module_fun = mp_compile(&parse_tree, lex->source_name, 0, asRepl == 0 ? false : true);
        nlr_pop();
        return module_fun;
    } else {
        handle_uncaught_exception(nlr.ret_val);
        return 0;
    }
}

int mp_run_execfun(void *moduleFun) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_call_function_0(moduleFun);
        if (MP_STATE_VM(mp_pending_exception) != MP_OBJ_NULL) {
            mp_obj_t obj = MP_STATE_VM(mp_pending_exception);
            MP_STATE_VM(mp_pending_exception) = MP_OBJ_NULL;
            nlr_raise(obj);
        }
        nlr_pop();
        return 0;
    } else {
        return handle_uncaught_exception(nlr.ret_val);
    }
}

//see unix/main.c
#define LEX_SRC_STR (1)
#define LEX_SRC_FILENAME (3)

int mp_run_execstring(const char *string, int asRepl){
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, string, strlen(string), false);
    return execute_from_lexer(LEX_SRC_STR, string, MP_PARSE_SINGLE_INPUT, asRepl == 0 ? false : true);
}

int mp_run_execfile(const char *file){
    char pathbuf[PATH_MAX];
    char *filepath = realpath(file, pathbuf);
    if(!filepath)
        return 2;
  
    size_t len;
    mp_obj_t *path_items;
    mp_obj_list_get(mp_sys_path, &len, &path_items);
    const char *basedir = strrchr(filepath, '/');
    path_items[0] = MP_OBJ_NEW_QSTR(qstr_from_strn(filepath, basedir - filepath));

    return execute_from_lexer(LEX_SRC_FILENAME, file, MP_PARSE_FILE_INPUT, false);
}

void mp_run_deinit(){
    mp_deinit();
}
