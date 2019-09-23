try:
    import ujson as json
    from uio import StringIO
    json.dumps(1, allow_nan=True)
except TypeError:  # No support for allow_nan in his build.
    print("SKIP")
    raise SystemExit
except ImportError:
    try:
        import json
        from io import StringIO
    except ImportError:
        print("SKIP")
        raise SystemExit


def DoDump(value, **kwargs):
  s = StringIO()
  json.dump(value, s, **kwargs)
  return s.getvalue()

def TestDumping(value):
  for f in [json.dumps, DoDump]:
    print(f(value))
    print(f(value, allow_nan=True))
    try:
        print(f(value, allow_nan=False))
    except ValueError:
        print('ValueError')


for n in [float("nan"), float("inf"), float("-inf")]:
    TestDumping(n)

for n in ["NaN", "Infinity", "-Infinity"]:
    print(json.loads(n))
    print(json.load(StringIO(n)))
    try:
        json.loads(n.lower())
    except ValueError:
        print('ValueError')
