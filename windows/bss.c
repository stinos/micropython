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

#include "mpconfig.h"
#include "misc.h"
#include "nlr.h"
#include "qstr.h"
#include "obj.h"
#include <windows.h>
#ifdef __INTIME__
#include <stdio.h>
#include <iwin32.h>
#include <rt.h>
#endif

IMAGE_NT_HEADERS *header_from_memory(BYTE *base_addr) {
    IMAGE_DOS_HEADER *dos_header = (IMAGE_DOS_HEADER*)base_addr;
    if (dos_header->e_magic != 0x5a4d) {
        return NULL;
    }
    return (IMAGE_NT_HEADERS*)(base_addr + dos_header->e_lfanew);
}

IMAGE_SECTION_HEADER *find_section(IMAGE_NT_HEADERS *nt_header, const char *name) {
    int i;
    IMAGE_SECTION_HEADER *section = IMAGE_FIRST_SECTION(nt_header);
    for (i = 0; i < nt_header->FileHeader.NumberOfSections; ++i) {
        if (strcmp((const char*)section->Name, name) == 0) {
            return section;
        }
        ++section;
    }
    return NULL;
}

bool section_boundaries(IMAGE_NT_HEADERS *nt_header, IMAGE_SECTION_HEADER *section, char **start, char **end) {
    if (section == NULL) {
        return false;
    }
    *start = (char*)(nt_header->OptionalHeader.ImageBase + section->VirtualAddress);
    *end = *start + section->Misc.VirtualSize;
    return true;
}

bool section_boundaries_from_memory(BYTE *base_addr, const char *section, char **base, char **start, char **end) {
    IMAGE_NT_HEADERS *nt_header = header_from_memory(base_addr);
    if (nt_header == NULL) {
        return false;
    }
    if (base !=NULL) {
        *base = (char*) nt_header->OptionalHeader.ImageBase;
    }
    return section_boundaries(nt_header, find_section(nt_header, section), start, end);
}

BYTE *base_address_from_module(const char *module) {
    return (BYTE*)GetModuleHandleA(module);
}

#ifndef __INTIME__
bool section_boundaries_from_module(const char *module, const char *section, char **start, char **end) {
    BYTE* base_addr = base_address_from_module(module);
    return section_boundaries_from_memory(base_addr, section, NULL, start, end);
}
#else
bool section_boundaries_from_file(const char *module, const char *section, char** base, char **start, char **end) {
    FILE *fp = fopen(module, "rb");
    size_t numBytes = 0x440;
    BYTE *buf = malloc(numBytes);
    size_t numRead = fread(buf, 1, numBytes, fp);
    fclose(fp);
    if (numRead != numBytes) {
        return false;
    }
    bool ok = section_boundaries_from_memory(buf, section, base, start, end);
    free(buf);
    return ok;
}

//INTime strips the PE header when loading shared libraries so we must get it from file.
//Normal procedure is to use MapViewOfFile and the likes but that is not available on INTime,
//so we just read the raw PE header from the module on disk (see section_boundaries_from_file)
//and then correct the section addresses with the offset of the loaded module (end of section_boundaries_from_module)
bool section_boundaries_from_module(const char *module, const char *section, char **start, char **end) {
    RTHANDLE mod = GetRtModuleHandle((char*)module);
    char fullPath[MAX_PATH];
    GetRtModuleFilename(mod, fullPath, sizeof(fullPath));

    char *base_addr = 0;
    if (!section_boundaries_from_file(fullPath, section, &base_addr, start, end)) {
        return false;
    }

    MODULEINFO moduleInfo;
    if(mod == BAD_RTHANDLE || !GetRtModuleInformation(mod, &moduleInfo)) {
        return false;
    }
    mp_uint_t virtualBase = (mp_uint_t) base_addr;
    *start -=  virtualBase;
    *end -= virtualBase;
    mp_uint_t actualBase = (mp_uint_t) moduleInfo.lpBaseOfDll;
    *start +=  actualBase;
    *end += actualBase;
    return true;
}
#endif

char *bss_start = 0;
char *bss_end = 0;

//MSVC has no __bss_start and _end but we can get accurate section info from the PE header.
//The standard .bss section is appended to the standard .data section however so it cannot
//be looked up by name. To deal with that we put all uPy static variables in a named section.
void getbss() {
    bool ok = section_boundaries_from_module(MICROPY_PORT_COREMODULE, MICROPY_PORT_BSSSECTION, &bss_start, &bss_end);
    if (!ok) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Could not lookup section boundaries"));
    }
}
