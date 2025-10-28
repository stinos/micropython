# This doesn't quite test everything but just serves to verify that basic syntax works,
# which for MicroPython means everything typing-related should be ignored.
import sys

try:
    import typing
except ImportError:
    print("SKIP")
    raise SystemExit

from typing import List, Tuple, Iterable, NewType, TypeVar, Union, Generic

# Available with MICROPY_PY_TYPING_EXTRA_MODULES. Skipped for Python3 since the test runner only
# loads core modules.
if "micropython" in sys.implementation.name:
    try:
        import typing_extensions
    except ImportError:
        print("- [ ] FIXME: typing_extensions not available")
        from typing import Any

        typing_extensions = None
else:
    from typing import Any

    typing_extensions = None

# Available with MICROPY_PY_TYPING_EXTRA_MODULES and MICROPY_MODULE_BUILTIN_SUBPACKAGES.
try:
    import collections.abc

    collections.abc.Sequence
except ImportError:
    print("- [ ] FIXME: collections.abc not available")

import sys

# If this is available verify it works, and try the other modules as well.
if typing_extensions is not None:
    from typing_extensions import Any
    import __future__
    from abc import abstractmethod

    getattr(__future__, "annotations")


# Verify assignment is not possible.
try:
    typing[0] = None
    print("- [ ] FIXME: 'module' object does not support item assignment")
except TypeError:
    pass

try:
    List.a = None
    print("- [ ] FIXME: cannot set 'a' attribute of immutable type 'list'")
except (AttributeError, TypeError):
    pass


MyAlias = str
Vector: typing.List[float]
UserId = NewType("UserId", int)
T = TypeVar("T", int, float, complex)

hintedGlobal: Any = None


def func_with_hints(c: int, b: MyAlias, a: Union[int, None], lst: List[float] = [0.0]) -> Any:
    pass


# Misc
try:

    class MyMeta(type): ...

    class MyABC(metaclass=MyMeta):
        def __init__(self):
            pass

    print(MyABC())

except Exception as e:
    print("- [ ] FIXME: metaclass not supported", type(e), e)
