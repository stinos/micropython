#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include <Windows.h>

typedef mp_obj_module_t* (*fun)();

mp_obj_module_t* LoadDynLib(const char *mod_name,vstr_t *path) {
    HMODULE hmod = LoadLibrary(vstr_str(path));
    if (!hmod) {
        return NULL;
    }
    const size_t nameLen = strlen(mod_name);
    char *initFunc = (char *)alloca(nameLen+5);
    memcpy(initFunc, "init", 4);
    strcpy(initFunc + 4, mod_name);
    fun f = (fun) GetProcAddress(hmod, initFunc);
    if( !f ) {
        return NULL;
    }
    return f();
}
