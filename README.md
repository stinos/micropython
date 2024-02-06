[![Unix CI badge](https://github.com/micropython/micropython/actions/workflows/ports_unix.yml/badge.svg)](https://github.com/micropython/micropython/actions?query=branch%3Amaster+event%3Apush) [![STM32 CI badge](https://github.com/micropython/micropython/actions/workflows/ports_stm32.yml/badge.svg)](https://github.com/micropython/micropython/actions?query=branch%3Amaster+event%3Apush) [![Docs CI badge](https://github.com/micropython/micropython/actions/workflows/docs.yml/badge.svg)](https://docs.micropython.org/) [![codecov](https://codecov.io/gh/micropython/micropython/branch/master/graph/badge.svg?token=I92PfD05sD)](https://codecov.io/gh/micropython/micropython)

The (windows-)pyd branch of MicroPython
=======================================
This fork of MicroPython (original readme below) allows building and importing
of native modules in a CPython-like way: `import <module_name>` will consider
the dynamic library `<module_name>.pyd` on windows or `<module_name>.so` on unix
for import (in case `<module_name>.py` is not found), load it, and call it's
`init_<module_name>()` function which should return a `mp_obj_module_t*`,
which is then added to the scope.

This fork gets  regularly rebased on the upstream.

**Example for the unix port**
- create a file mymodule.c with the following content
``` C
#include "py/runtime.h"

static mp_obj_t foo() {
  return MP_OBJ_NEW_QSTR(qstr_from_str("hello from foo"));
}
static MP_DEFINE_CONST_FUN_OBJ_0(foo_obj, foo);

mp_obj_module_t *init_mymodule() {
  mp_obj_t module = mp_obj_new_module(qstr_from_str("mymodule"));
  mp_store_attr(module, qstr_from_str("foo"), MP_OBJ_FROM_PTR(&foo_obj));
  return module;
}
```
- set a shell variable UPYDIR to the location of this repository and build MicroPython
```
export UPYDIR=/path/to/micropython
make -C $UPYDIR/ports/unix
```
- build the module and place it in the default search path
```
gcc -fPIC -I$UPYDIR -I$UPYDIR/ports/unix -I$UPYDIR/ports/unix/build -c mymodule.c -o mymodule.o
mkdir -p ~/.micropython/lib
gcc -shared -o ~/.micropython/lib/mymodule.so mymodule.o
```
- run micropython and import the module
```
$UPYDIR/ports/unix/micropython
>>> import mymodule
>>> mymodule.foo()
'hello from foo'
```

**Example for the windows port**

When using project files import [extmodule.props](https://github.com/stinos/micropython/blob/windows-pyd/ports/windows/msvc/extmodule.props) to get all options set correctly. Alternatively, here's a commandline sample:
- create a file mymodule.c with the following content
``` C
#include "py/runtime.h"

static mp_obj_t foo(void) {
  return MP_OBJ_NEW_QSTR(qstr_from_str("hello from foo"));
}

__declspec(dllexport) mp_obj_module_t *init_mymodule() {
  mp_obj_t module = mp_obj_new_module(qstr_from_str("mymodule"));
  //MP_DEFINE_CONST_FUN_OBJ_0 won't compile since mp_type_fun_builtin_0 is
  //dynamically imported so create the function on the heap instead.
  mp_obj_fun_builtin_fixed_t *foo_obj = m_new_obj(mp_obj_fun_builtin_fixed_t);
  foo_obj->base.type = &mp_type_fun_builtin_0;
  foo_obj->fun._0 = foo;
  mp_store_attr(module, qstr_from_str("foo"), MP_OBJ_FROM_PTR(foo_obj));
  return module;
}
```
- set a shell variable UPYDIR to the location of this repository and build MicroPython
```
$UPYDIR='/path/to/micropython'
msbuild $UPYDIR/ports/windows/micropythoncore.vcxproj
msbuild $UPYDIR/ports/windows/micropython.vcxproj
```
- build the module
```
cl /c /I$UPYDIR /I$UPYDIR/ports/windows /I$UPYDIR/ports/windows/msvc /I$UPYDIR/ports/windows/build mymodule.c
link /DLL /OUT:mymodule_d.pyd $UPYDIR/ports/windows/build/Debugx64/micropythoncore.lib mymodule.obj
```
- run micropython and import the module
```
$UPYDIR/ports/windows/micropython
>>> import mymodule
>>> mymodule.foo()
'hello from foo'
```

**Other examples**

The [micropython-wrap repository](https://github.com/stinos/micropython-wrap) has a sample module, Makefile and VS/msbuild project files.

**Note on module search path**

On unix `dlopen()` is used to load libraries so it uses a different search path than what MicroPython uses to find modules. This isn't a problem when full paths are passed
to `dlopen()` which is normally the case, except when MicroPython finds the .so file in
the current directory for instance: to make that work use `LD_LIBRARY_PATH=$(pwd)` to have the loader
find the .so file in the current directory.

On windows the current directory is automatically searched for dynamic libraries so no problem there.

**Note on module names**

The CPython naming scheme is followed on windows so when `_DEBUG` is defined MicroPython looks for
`<module_name>_d.pyd` instead of `<module_name>.pyd`.

The MicroPython project
=======================
<p align="center">
  <img src="https://raw.githubusercontent.com/micropython/micropython/master/logo/upython-with-micro.jpg" alt="MicroPython Logo"/>
</p>

This is the MicroPython project, which aims to put an implementation
of Python 3.x on microcontrollers and small embedded systems.
You can find the official website at [micropython.org](http://www.micropython.org).

WARNING: this project is in beta stage and is subject to changes of the
code-base, including project-wide name changes and API changes.

MicroPython implements the entire Python 3.4 syntax (including exceptions,
`with`, `yield from`, etc., and additionally `async`/`await` keywords from
Python 3.5 and some select features from later versions). The following core
datatypes are provided: `str`(including basic Unicode support), `bytes`,
`bytearray`, `tuple`, `list`, `dict`, `set`, `frozenset`, `array.array`,
`collections.namedtuple`, classes and instances. Builtin modules include
`os`, `sys`, `time`, `re`, and `struct`, etc. Some ports have support for
`_thread` module (multithreading), `socket` and `ssl` for networking, and
`asyncio`. Note that only a subset of Python 3 functionality is implemented
for the data types and modules.

MicroPython can execute scripts in textual source form (.py files) or from
precompiled bytecode (.mpy files), in both cases either from an on-device
filesystem or "frozen" into the MicroPython executable.

MicroPython also provides a set of MicroPython-specific modules to access
hardware-specific functionality and peripherals such as GPIO, Timers, ADC,
DAC, PWM, SPI, I2C, CAN, Bluetooth, and USB.

Getting started
---------------

See the [online documentation](https://docs.micropython.org/) for the API
reference and information about using MicroPython and information about how
it is implemented.

We use [GitHub Discussions](https://github.com/micropython/micropython/discussions)
as our forum, and [Discord](https://discord.gg/RB8HZSAExQ) for chat. These
are great places to ask questions and advice from the community or to discuss your
MicroPython-based projects.

For bugs and feature requests, please [raise an issue](https://github.com/micropython/micropython/issues/new/choose)
and follow the templates there.

For information about the [MicroPython pyboard](https://store.micropython.org/pyb-features),
the officially supported board from the
[original Kickstarter campaign](https://www.kickstarter.com/projects/214379695/micro-python-python-for-microcontrollers),
see the [schematics and pinouts](http://github.com/micropython/pyboard) and
[documentation](https://docs.micropython.org/en/latest/pyboard/quickref.html).

Contributing
------------

MicroPython is an open-source project and welcomes contributions. To be
productive, please be sure to follow the
[Contributors' Guidelines](https://github.com/micropython/micropython/wiki/ContributorGuidelines)
and the [Code Conventions](https://github.com/micropython/micropython/blob/master/CODECONVENTIONS.md).
Note that MicroPython is licenced under the MIT license, and all contributions
should follow this license.

About this repository
---------------------

This repository contains the following components:
- [py/](py/) -- the core Python implementation, including compiler, runtime, and
  core library.
- [mpy-cross/](mpy-cross/) -- the MicroPython cross-compiler which is used to turn scripts
  into precompiled bytecode.
- [ports/](ports/) -- platform-specific code for the various ports and architectures that MicroPython runs on.
- [lib/](lib/) -- submodules for external dependencies.
- [tests/](tests/) -- test framework and test scripts.
- [docs/](docs/) -- user documentation in Sphinx reStructuredText format. This is used to generate the [online documentation](http://docs.micropython.org).
- [extmod/](extmod/) -- additional (non-core) modules implemented in C.
- [tools/](tools/) -- various tools, including the pyboard.py module.
- [examples/](examples/) -- a few example Python scripts.

"make" is used to build the components, or "gmake" on BSD-based systems.
You will also need bash, gcc, and Python 3.3+ available as the command `python3`.
Some ports (rp2 and esp32) additionally use CMake.

Supported platforms & architectures
-----------------------------------

MicroPython runs on a wide range of microcontrollers, as well as on Unix-like
(including Linux, BSD, macOS, WSL) and Windows systems.

Microcontroller targets can be as small as 256kiB flash + 16kiB RAM, although
devices with at least 512kiB flash + 128kiB RAM allow a much more
full-featured experience.

The [Unix](ports/unix) and [Windows](ports/windows) ports allow both
development and testing of MicroPython itself, as well as providing
lightweight alternative to CPython on these platforms (in particular on
embedded Linux systems).

The ["minimal"](ports/minimal) port provides an example of a very basic
MicroPython port and can be compiled as both a standalone Linux binary as
well as for ARM Cortex M4. Start with this if you want to port MicroPython to
another microcontroller. Additionally the ["bare-arm"](ports/bare-arm) port
is an example of the absolute minimum configuration, and is used to keep
track of the code size of the core runtime and VM.

In addition, the following ports are provided in this repository:
 - [cc3200](ports/cc3200) -- Texas Instruments CC3200 (including PyCom WiPy).
 - [esp32](ports/esp32) -- Espressif ESP32 SoC (including ESP32S2, ESP32S3, ESP32C3, ESP32C6).
 - [esp8266](ports/esp8266) -- Espressif ESP8266 SoC.
 - [mimxrt](ports/mimxrt) -- NXP m.iMX RT (including Teensy 4.x).
 - [nrf](ports/nrf) -- Nordic Semiconductor nRF51 and nRF52.
 - [pic16bit](ports/pic16bit) -- Microchip PIC 16-bit.
 - [powerpc](ports/powerpc) -- IBM PowerPC (including Microwatt)
 - [qemu](ports/qemu) -- QEMU-based emulated target (for testing)
 - [renesas-ra](ports/renesas-ra) -- Renesas RA family.
 - [rp2](ports/rp2) -- Raspberry Pi RP2040 (including Pico and Pico W).
 - [samd](ports/samd) -- Microchip (formerly Atmel) SAMD21 and SAMD51.
 - [stm32](ports/stm32) -- STMicroelectronics STM32 family (including F0, F4, F7, G0, G4, H7, L0, L4, WB)
 - [webassembly](ports/webassembly) -- Emscripten port targeting browsers and NodeJS.
 - [zephyr](ports/zephyr) -- Zephyr RTOS.

The MicroPython cross-compiler, mpy-cross
-----------------------------------------

Most ports require the [MicroPython cross-compiler](mpy-cross) to be built
first.  This program, called mpy-cross, is used to pre-compile Python scripts
to .mpy files which can then be included (frozen) into the
firmware/executable for a port.  To build mpy-cross use:

    $ cd mpy-cross
    $ make

External dependencies
---------------------

The core MicroPython VM and runtime has no external dependencies, but a given
port might depend on third-party drivers or vendor HALs. This repository
includes [several submodules](lib/) linking to these external dependencies.
Before compiling a given port, use

    $ cd ports/name
    $ make submodules

to ensure that all required submodules are initialised.
