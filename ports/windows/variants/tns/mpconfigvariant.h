#define MICROPY_ENABLE_SCHEDULER                  (0)
#define MICROPY_PY_BUILTINS_HELP                  (1)
#define MICROPY_PY_BUILTINS_HELP_MODULES          (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW_ITEMSIZE   (1)
#define MICROPY_PY_BUILTINS_NEXT2                 (1)
#define MICROPY_PY_BUILTINS_DESCRIPTOR            (1)
#define MICROPY_PY_BUILTINS_RANGE_BINOP           (1)
#define MICROPY_PY_BUILTINS_SLICE_INDICES         (1)
#define MICROPY_PY_COLLECTIONS_NAMEDTUPLE__ASDICT (1)
#define MICROPY_PY_MATH_CONSTANTS                 (1)
#define MICROPY_PY_MATH_FACTORIAL                 (1)
#define MICROPY_PY_SYS_EXECUTABLE                 (1)
#define MICROPY_PY_SYS_INTERN                     (1)
#define MICROPY_PY_PLATFORM                       (1)
#define MICROPY_PY_RANDOM_EXTRA_FUNCS             (1)
#define MICROPY_PY_RE_MATCH_GROUPS                (1)
#define MICROPY_PY_RE_MATCH_SPAN_START_END        (1)
#define MICROPY_PY_RE_SUB                         (1)
#define MICROPY_REPL_EMACS_WORDS_MOVE             (1)
#define MICROPY_REPL_EMACS_EXTRA_WORDS_MOVE       (1)

extern const struct _mp_obj_type_t mp_type_ConnectionError;
extern const struct _mp_obj_type_t mp_type_TimeoutError;
#define MICROPY_PORT_EXTRA_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_ConnectionError), MP_ROM_PTR(&mp_type_ConnectionError) }, \
    { MP_ROM_QSTR(MP_QSTR_TimeoutError), MP_ROM_PTR(&mp_type_TimeoutError) }, \
// Extra line to avoid "backslash-newline at end of file" warning.
