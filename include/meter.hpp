#pragma once

#include <cstdint>
#include <string>

struct Pulse {
    std::uint64_t sequence{};
    std::uint64_t timestamp_epoch{};
    double energy_wh{};
};

struct MeterReading {
    std::uint64_t sequence{};
    std::uint64_t timestamp_epoch{};
    std::uint64_t total_pulses{};
    double interval_energy_wh{};
    double cumulative_energy_kwh{};
    double estimated_power_w{};
};

class SmartMeter {
public:
    explicit SmartMeter(double pulse_energy_wh);

    MeterReading process_pulse(const Pulse& pulse);
    std::uint64_t total_pulses() const;
    double cumulative_energy_kwh() const;

private:
    double pulse_energy_wh_;
    std::uint64_t total_pulses_{0};
    double cumulative_energy_wh_{0.0};
    std::uint64_t previous_timestamp_{0};
};

Pulse make_pulse(std::uint64_t sequence,
                 std::uint64_t timestamp_epoch,
                 double energy_wh);
