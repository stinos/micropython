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
#include "extmod/vfs.h"
#include "extmod/vfs_posix.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// From unix/main.c: its execute_from_lexer has a lot of what we need so reuse it.
#define LEX_SRC_STR (1)
#define LEX_SRC_FILENAME (3)

extern int execute_from_lexer(int source_kind, const void *source, mp_parse_input_kind_t input_kind, bool is_repl);
extern int handle_uncaught_exception(mp_obj_base_t *exc);

#ifdef _WIN32
#define PATHLIST_SEP_CHAR ';'
#else
#define PATHLIST_SEP_CHAR ':'
#endif

void mp_run_init(char *heap, size_t heap_size, char *stack, size_t stack_size) {
    mp_stack_ctrl_init();
    mp_stack_set_limit(40000 * ((sizeof(void *)) / 4));

    #if MICROPY_ENABLE_GC
    gc_init(heap, heap + heap_size);
    #endif

    #if MICROPY_ENABLE_PYSTACK
    mp_pystack_init(stack, stack + stack_size);
    #endif

    mp_init();

    #if MICROPY_VFS_POSIX
    // Mount the host FS at the root of our internal VFS
    mp_obj_t args[2] = {
        MP_OBJ_TYPE_GET_SLOT(&mp_type_vfs_posix, make_new)(&mp_type_vfs_posix, 0, 0, NULL),
        MP_OBJ_NEW_QSTR(MP_QSTR__slash_),
    };
    mp_vfs_mount(2, args, (mp_map_t *)&mp_const_empty_map);
    MP_STATE_VM(vfs_cur) = MP_STATE_VM(vfs_mount_table);
    #endif

    // sys.path starts as [""]
    mp_sys_path = mp_obj_new_list(0, NULL);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_));

    // Add colon-separated entries from MICROPYPATH.
    char *home = getenv("HOME");
    char *path = getenv("MICROPYPATH");
    if (path == NULL) {
        path = MICROPY_PY_SYS_PATH_DEFAULT;
    }
    if (*path == PATHLIST_SEP_CHAR) {
        // First entry is empty. We've already added an empty entry to sys.path, so skip it.
        ++path;
    }
    bool path_remaining = *path;
    while (path_remaining) {
        char *path_entry_end = strchr(path, PATHLIST_SEP_CHAR);
        if (path_entry_end == NULL) {
            path_entry_end = path + strlen(path);
            path_remaining = false;
        }
        if (path[0] == '~' && path[1] == '/' && home != NULL) {
            // Expand standalone ~ to $HOME
            int home_l = strlen(home);
            vstr_t vstr;
            vstr_init(&vstr, home_l + (path_entry_end - path - 1) + 1);
            vstr_add_strn(&vstr, home, home_l);
            vstr_add_strn(&vstr, path + 1, path_entry_end - path - 1);
            mp_obj_list_append(mp_sys_path, mp_obj_new_str_from_vstr(&vstr));
        } else {
            mp_obj_list_append(mp_sys_path, mp_obj_new_str_via_qstr(path, path_entry_end - path));
        }
        path = path_entry_end + 1;
    }

    mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_argv), 0);
}

int mp_run_parse(const char *string, int is_repl, mp_obj_t *compiled_object) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, string, strlen(string), 0);
        mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_SINGLE_INPUT);
        *compiled_object = mp_compile(&parse_tree, lex->source_name, is_repl == 0 ? false : true);
        nlr_pop();
        return 0;
    } else {
        return handle_uncaught_exception(nlr.ret_val);
    }
}

int mp_run_execfun(mp_obj_t compiled_object, mp_obj_t *return_value) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t result = mp_call_function_0(compiled_object);
        mp_handle_pending(true);
        nlr_pop();
        if (return_value) {
            *return_value = result;
        }
        return 0;
    } else {
        return handle_uncaught_exception(nlr.ret_val);
    }
}


int mp_run_execstring(const char *string, int is_repl) {
    return execute_from_lexer(LEX_SRC_STR, string, MP_PARSE_SINGLE_INPUT, is_repl == 0 ? false : true);
}

int mp_run_evalstring(const char *string, mp_obj_t *return_value) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, string, strlen(string), 0);
        mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_EVAL_INPUT);
        mp_obj_t module_fun = mp_compile(&parse_tree, lex->source_name, false);
        mp_obj_t result = mp_call_function_0(module_fun);
        mp_handle_pending(true);
        nlr_pop();
        if (return_value) {
            *return_value = result;
        }
        return 0;
    } else {
        return handle_uncaught_exception(nlr.ret_val);
    }
}

int mp_run_execfile(const char *file) {
    char pathbuf[PATH_MAX];
    char *filepath = realpath(file, pathbuf);
    if (!filepath) {
        return 2;
    }

    size_t len;
    mp_obj_t *path_items;
    mp_obj_list_get(mp_sys_path, &len, &path_items);
    const char *basedir = strrchr(filepath, '/');
    path_items[0] = MP_OBJ_NEW_QSTR(qstr_from_strn(filepath, basedir - filepath));

    return execute_from_lexer(LEX_SRC_FILENAME, file, MP_PARSE_FILE_INPUT, false);
}

void mp_run_deinit() {
    mp_deinit();
}
