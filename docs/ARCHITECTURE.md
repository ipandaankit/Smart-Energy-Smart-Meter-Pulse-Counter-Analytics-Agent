# Architecture

## Modules

### 1. Virtual load generator

`src/main.cpp`

Creates realistic power values using a periodic load pattern plus random noise. It also injects known abnormal conditions for testing.

### 2. Smart meter

`src/meter.cpp`

Responsibilities:

- accept pulses
- validate pulse energy
- increment pulse counter
- accumulate energy
- calculate interval power

### 3. Analytics

`src/analyzer.cpp`

Responsibilities:

- classify readings
- identify high consumption
- identify low consumption
- identify invalid/gapped power intervals
- calculate summary statistics

### 4. Telemetry

`src/telemetry.cpp`

Responsibilities:

- initialize output files
- write CSV telemetry
- write event records
- write JSON summary

### 5. Configuration

`src/config.cpp`

Loads key/value configuration from `config/smart_meter.conf`.

## Data flow

```text
Power Model
    |
    v
Energy Calculation
    |
    v
Pulse
    |
    v
SmartMeter
    |
    +--> pulse counter
    +--> cumulative kWh
    +--> estimated watts
    |
    v
Analyzer
    |
    +--> normal
    +--> high consumption
    +--> low consumption
    +--> pulse gap
    |
    v
Telemetry Storage
```
