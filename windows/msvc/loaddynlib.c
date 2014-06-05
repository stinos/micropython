#include "py/mpconfig.h"
#include "py/nlr.h"
#include "py/misc.h"
#include "py/qstr.h"
#include "py/obj.h"
#include <windows.h>

typedef mp_obj_module_t* (*fun)();

mp_obj_module_t* mp_load_dynlib(const char *mod_name,vstr_t *path) {
    // Disable error dialogs which might result from trying to load
    // libraries for which not all dependencies are found
    DWORD errMode;
    SetThreadErrorMode(SEM_FAILCRITICALERRORS, &errMode);
    HMODULE hmod = LoadLibrary(vstr_null_terminated_str(path));
    SetThreadErrorMode(errMode, NULL);
    if (!hmod) {
        return NULL;
    }
    const size_t nameLen = strlen(mod_name);
    char *initFunc = (char*)alloca(nameLen+6);
    memcpy(initFunc, "init_", 5);
    strcpy(initFunc + 5, mod_name);
    fun f = (fun) GetProcAddress(hmod, initFunc);
    if( !f ) {
        FreeLibrary(hmod);
        return NULL;
    }
    return f();
}
