"""
categories: Core,Classes
description: Defining a class with a metaclass is not possible.
cause: Currently not implemented to limit size and complexity of the runtime.
workaround: Use composition or class decorators instead of metaclasses. See https://realpython.com/python-metaclasses/#is-this-really-necessary
"""

from abc import ABCMeta


class MyABC(metaclass=ABCMeta):
    pass
