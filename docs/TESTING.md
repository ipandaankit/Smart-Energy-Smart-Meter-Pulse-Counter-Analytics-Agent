# Testing

The project contains unit tests in `tests/test_main.cpp`.

The tests cover:

1. Pulse counting.
2. Cumulative energy calculation.
3. Power estimation.
4. High-consumption detection.
5. Low-consumption detection.
6. Normal reading classification.
7. Summary statistics.

Run:

```bash
cmake -S . -B build
cmake --build build -j$(nproc)
cd build
ctest --output-on-failure
```
