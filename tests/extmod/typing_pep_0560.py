try:
    from typing import TYPE_CHECKING
except Exception:
    print("SKIP")
    raise SystemExit

print("# Python 3.7")
print("### PEP 560 - Type Hinting Generics In Standard Collections")

print("Specification")


print("__class_getitem__")


class MyList:
    def __getitem__(self, index):
        return index + 1

    def __class_getitem__(cls, item):
        return f"{cls.__name__}[{item.__name__}]"


class MyOtherList(MyList):
    pass


assert MyList()[0] == 1

try:
    print(MyList[int])
    print(MyOtherList[int])
except Exception as e:
    print("- [ ] FIXME: __class_getitem__ not supported:", e)


# -------------------------


class GenericAlias:
    def __init__(self, origin, item):
        self.origin = origin
        self.item = item

    def __mro_entries__(self, bases):
        return (self.origin,)


class NewList:
    def __class_getitem__(cls, item):
        return GenericAlias(cls, item)


try:

    class Tokens(NewList[int]): ...

    print(NewList in Tokens.__bases__)
    print(GenericAlias in Tokens.__orig_bases__)
    print([x in Tokens.__mro__ for x in [Tokens, NewList, object]])
except Exception as e:
    print("- [ ] FIXME: __mro_entries__ not supported:", e)

print("-----")
