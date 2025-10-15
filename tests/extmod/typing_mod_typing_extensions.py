try:
    # typing_extensions MUST exist when typing is available
    import typing_extensions
except ImportError:
    print("SKIP")
    raise SystemExit

print("# Python 3.5")
print("### module typing_extensions")
# https://typing-extensions.readthedocs.io/

# No test for runtime behaviour

print("Special typing primitives")

import typing_extensions

try:
    from typing_extensions import *

except ImportError:
    print("- [ ] FIXME: add typing_extensions module")

# Do not test for all the individual names, just that the module exists
# as the different CPython versions have different subsets of names

print("-----")
