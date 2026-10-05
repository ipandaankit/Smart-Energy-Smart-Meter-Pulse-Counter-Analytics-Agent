# Smart Energy Smart-Meter Pulse Counter & Analytics Agent

A fully software-based C++17 smart-meter simulation designed to run natively inside WSL Ubuntu.

## What it does

The system simulates an energy meter that:

1. Generates virtual electrical power measurements.
2. Converts power into energy pulses.
3. Counts and validates meter pulses.
4. Calculates cumulative energy in kWh.
5. Estimates instantaneous power from pulse intervals.
6. Detects high-consumption and low-consumption events.
7. Persists raw telemetry to CSV.
8. Persists detected events to CSV/log output.
9. Produces a JSON analytics summary.
10. Includes automated C++ tests.

No physical smart-meter hardware is required.

## Architecture

```text
+---------------------+
| Virtual Load Model  |
+----------+----------+
           |
           v
+---------------------+
| Pulse Generator     |
+----------+----------+
           |
           v
+---------------------+
| SmartMeter Counter  |
| Energy Accumulator  |
+----------+----------+
           |
           v
+---------------------+
| Analytics Engine    |
| Threshold Detection|
+----------+----------+
           |
       +---+---+
       |       |
       v       v
    CSV/Log   JSON
    Telemetry Summary
```

## Requirements

Ubuntu/WSL with:

- GCC/G++
- CMake 3.16+
- C++17 standard library

Install:

```bash
sudo apt update
sudo apt install -y build-essential cmake
```

## Build

From the project root:

```bash
cmake -S . -B build
cmake --build build -j$(nproc)
```

## Run

Normal simulation:

```bash
./build/smart_meter_agent
```

Run fewer samples:

```bash
./build/smart_meter_agent --samples 20
```

Use a custom config:

```bash
./build/smart_meter_agent --config config/smart_meter.conf
```

Real-time simulation:

```bash
./build/smart_meter_agent --realtime
```

Help:

```bash
./build/smart_meter_agent --help
```

## Test

```bash
cd build
ctest --output-on-failure
```

Expected:

```text
All tests passed.
100% tests passed
```

## Output

After execution:

```text
data/
├── telemetry.csv
├── events.log
└── summary.json
```

### telemetry.csv

Contains:

- sequence
- timestamp
- total pulse count
- interval energy
- cumulative energy
- estimated power

### events.log

Contains detected:

- HIGH_CONSUMPTION
- LOW_CONSUMPTION
- PULSE_GAP
- NORMAL

### summary.json

Contains:

- number of samples
- high-consumption event count
- low-consumption event count
- pulse-gap event count
- total energy
- average power
- peak power

## Important implementation detail

For an interval of `t` seconds:

```text
Energy(Wh) = Power(W) × t / 3600
```

The simulated meter accumulates these energy values. Power can then be reconstructed from the energy pulse and interval duration.

## Demo anomaly injection

The simulator intentionally injects abnormal values at selected sample numbers so that the analytics subsystem can be demonstrated without external hardware:

- high-load spike
- low-load condition

This makes the project useful for demonstration and testing.

## Suggested viva explanation

**What is the project?**

It is a software-based smart energy meter agent. Instead of connecting a physical meter, it generates virtual energy pulses and processes them like an embedded smart-meter system.

**Why pulses?**

Many energy meters expose consumption as pulses. Each pulse represents a known amount of energy.

**What does the counter do?**

It counts incoming pulses and converts them into cumulative energy.

**What does the analytics agent do?**

It estimates power and detects abnormal consumption using configurable thresholds.

**Why C++?**

C++ is suitable for embedded and edge systems because it provides high performance, deterministic resource usage, and direct control over system resources.

## Project extensions

Possible future improvements:

- MQTT telemetry publishing
- Modbus/serial input
- SQLite storage
- Web dashboard
- Multiple virtual meters
- Demand forecasting
- Tariff/cost calculation
- Rolling-window anomaly detection
- Linux systemd service
- Docker deployment
