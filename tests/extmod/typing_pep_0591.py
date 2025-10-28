try:
    from typing import TYPE_CHECKING
except Exception:
    print("SKIP")
    raise SystemExit

print("# Python 3.8")
print("### PEP 0591 - Final qualifier for types")
# https://peps.python.org/pep-0591/

# Note this is a typechecker enforcement, not a runtime one.

print("The final decorator")

from typing import List, Sequence, final, overload


@final
class Base_1: ...


class Derived_1(Base_1):  # Error: Cannot inherit from final class "Base"
    ...


class Base:
    @overload
    def method(self) -> None: ...
    @overload
    def method(self, arg: int) -> int: ...
    @final
    def method(self, x=None):
        return x


print(Base().method(1))


class Base_2:
    @final
    def foo(self) -> None: ...


class Derived_2(Base_2):
    def foo(self) -> None:  # Error: Cannot override final attribute "foo"
        # (previously declared in base class "Base")
        ...


print(Base_2().foo())


print("The Final annotation")

from typing import Final

ID_1: Final[float] = 1
ID_2: Final = 2

print("Semantics and examples")

from typing import Final

RATE: Final = 3000


class Base:
    DEFAULT_ID: Final = 0


RATE = 300  # Error: can't assign to final attribute
Base.DEFAULT_ID = 1  # Error: can't override a final attribute


# -------------------

try:
    x: List[Final[int]] = []  # Error!
    print("- [ ] FIXME: Final cannot be used with container types")
except Exception:
    print("Expected: Final cannot be used with container types")


def fun(x: Final[List[int]]) -> None:  # Error!
    ...

fun(1)


# -------------------

x_2: Final = ["a", "b"]
x_2.append("c")  # OK
print(x_2)

y: Final[Sequence[str]] = ["a", "b"]
z: Final = ("a", "b")  # Also works


# -------------------
from typing import NamedTuple, Final

X: Final = "x"
Y: Final = "y"
N = NamedTuple("N", [(X, int), (Y, int)])


print("-----")
