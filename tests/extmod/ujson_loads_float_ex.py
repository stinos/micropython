try:
    import ujson as json
except ImportError:
    try:
        import json
    except ImportError:
        print("SKIP")
        raise SystemExit

print(json.loads("NaN"))
print(json.loads("Infinity"))
print(json.loads("-Infinity"))
