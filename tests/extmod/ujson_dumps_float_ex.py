try:
    import ujson as json
except ImportError:
    try:
        import json
    except ImportError:
        print("SKIP")
        raise SystemExit

print(json.dumps(float("nan")))
print(json.dumps(float("inf")))
print(json.dumps(float("-inf")))
