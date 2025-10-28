from math import e


try:
    import abc
except ImportError:
    print("SKIP")
    raise SystemExit

print("# Python 3.0+")
print("### module abc")
# https://peps.python.org/pep-3119/
# https://docs.python.org/3/library/abc.html#module-abc


print("PEP 3119 - abc")
try:
    from abc import ABC, ABCMeta
except Exception:
    print("- [ ] FIXME: from abc import ABC")

try:

    class MyABC(metaclass=ABCMeta): ...

except Exception as e:
    print("- [ ] FIXME: metaclass not supported:", e)

from typing import Sequence

try:
    assert issubclass(list, Sequence)
except Exception as e:
    print("- [ ] FIXME: issubclass(..., Sequence) etc not supported:", e)

print("@abc.abstractmethod")
try:
    from abc import ABC, abstractmethod
except Exception:
    print("- [ ] FIXME: from abc import abstractmethod")
    raise SystemExit


class C1(ABC):
    @abstractmethod
    def my_abstract_method(self, arg1): ...

    @classmethod
    @abstractmethod
    def my_abstract_classmethod(cls, arg2): ...

    @staticmethod
    @abstractmethod
    def my_abstract_staticmethod(arg3): ...

    @property
    @abstractmethod
    def my_abstract_property(self): ...

    @my_abstract_property.setter
    @abstractmethod
    def my_abstract_property(self, val): ...

    @abstractmethod
    def _get_x(self): ...

    @abstractmethod
    def _set_x(self, val): ...

    x = property(_get_x, _set_x)


# do not test deprecated abstractclassmethod, abstractstaticmethod, abstractproperty


print("abc.get_cache_token()")

try:
    from abc import get_cache_token

    a = get_cache_token()
    MyABC.register(tuple)
    b = get_cache_token()
    assert a != b
except Exception as e:
    print("- [ ] FIXME: get_cache_token at runtime not supported", e)


print("-----")
