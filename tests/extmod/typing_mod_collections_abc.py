from math import e


try:
    import collections.abc
except ImportError:
    print("SKIP")
    raise SystemExit

print("# Python 3.3+")
print("### module collections.abc")
# https://peps.python.org/pep-3119/
# https://docs.python.org/3/library/collections.abc.html#module-collections.abc

# No test for runtime behaviour

print("collections.abc")

from collections.abc import Container
from collections.abc import Hashable
from collections.abc import Iterable
from collections.abc import Iterator
from collections.abc import Reversible
from collections.abc import Generator
from collections.abc import Sized
from collections.abc import Callable
from collections.abc import Collection
from collections.abc import Sequence
from collections.abc import MutableSequence

# from collections.abc import ByteString # Deprecated since version 3.12,
from collections.abc import Set
from collections.abc import MutableSet
from collections.abc import Mapping
from collections.abc import MutableMapping
from collections.abc import MappingView
from collections.abc import KeysView
from collections.abc import ItemsView
from collections.abc import ValuesView
from collections.abc import Awaitable
from collections.abc import Coroutine
from collections.abc import AsyncIterable
from collections.abc import AsyncIterator
from collections.abc import AsyncGenerator
from collections.abc import Buffer


print("-----")
