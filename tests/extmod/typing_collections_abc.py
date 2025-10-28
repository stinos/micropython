print("Testing runtime aspects of collections.abc module")

try:
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
except ImportError:
    print("- [ ] FIXME: collections.abc should have Mapping, Sequence etc")
    try:
        from typing import Mapping, Sequence, Callable, Awaitable, Iterable
    except:
        print("SKIP")
        raise SystemExit


print("Testing : collections.abc.Mapping, Sequence")


class Employee: ...


def notify_by_email(employees: Sequence[Employee], overrides: Mapping[str, str]) -> None:
    pass


notify_by_email([], {})


print("Testing : collections.abc.Callable, Awaitable")


def feeder(get_next_item: Callable[[], str]) -> None: ...  # Body


def async_query(
    on_success: Callable[[int], None], on_error: Callable[[int, Exception], None]
) -> None: ...  # Body


async def on_update(value: str) -> None: ...  # Body


callback: Callable[[str], Awaitable[None]] = on_update

# ...


def concat(x: str, y: str) -> str:
    return x + y


x: Callable[..., str]
x = str  # OK
x = concat  # Also OK
print(concat(1, 2))


print("Testing : collections.abc.Iterable")


class Combiner:
    def __call__(self, *vals: bytes, maxlen: int | None = None) -> list[bytes]: ...


def batch_proc(data: Iterable[bytes], cb_results: Combiner) -> bytes:
    for item in data:
        pass
    return b"".join(cb_results(*data))


def good_cb(*vals: bytes, maxlen: int | None = None) -> list[bytes]:
    return [val[:maxlen] for val in vals if maxlen is not None]


batch_proc([], good_cb)  # OK
