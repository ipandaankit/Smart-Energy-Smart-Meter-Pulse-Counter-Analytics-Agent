# WSL Ubuntu Setup

## Install WSL

From Windows PowerShell as Administrator:

```powershell
wsl --install -d Ubuntu
```

Restart Windows if requested.

Open Ubuntu and update packages:

```bash
sudo apt update
sudo apt upgrade -y
```

Install build tools:

```bash
sudo apt install -y build-essential cmake
```

## Copy the project

If the ZIP is downloaded in Windows Downloads, it can usually be accessed from:

```bash
cd /mnt/c/Users/<YOUR_WINDOWS_USERNAME>/Downloads
```

Unzip:

```bash
unzip Domain1-Smart-Energy-Smart-Meter.zip
cd Domain1-Smart-Energy-Smart-Meter
```

If unzip is missing:

```bash
sudo apt install -y unzip
```

## Build

```bash
cmake -S . -B build
cmake --build build -j$(nproc)
```

## Execute

```bash
./build/smart_meter_agent
```

## Test

```bash
cd build
ctest --output-on-failure
```
