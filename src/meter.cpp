#include "meter.hpp"

#include <stdexcept>

SmartMeter::SmartMeter(double pulse_energy_wh)
    : pulse_energy_wh_(pulse_energy_wh) {
    if (pulse_energy_wh_ <= 0.0) {
        throw std::invalid_argument("Pulse energy must be positive");
    }
}

Pulse make_pulse(std::uint64_t sequence,
                 std::uint64_t timestamp_epoch,
                 double energy_wh) {
    if (energy_wh <= 0.0) {
        throw std::invalid_argument("Pulse energy must be positive");
    }
    return Pulse{sequence, timestamp_epoch, energy_wh};
}

MeterReading SmartMeter::process_pulse(const Pulse& pulse) {
    if (pulse.energy_wh <= 0.0) {
        throw std::invalid_argument("Invalid pulse energy");
    }

    ++total_pulses_;
    cumulative_energy_wh_ += pulse.energy_wh;

    double power = 0.0;
    if (previous_timestamp_ != 0 && pulse.timestamp_epoch > previous_timestamp_) {
        const double hours = static_cast<double>(pulse.timestamp_epoch - previous_timestamp_) / 3600.0;
        power = pulse.energy_wh / hours;
    }

    previous_timestamp_ = pulse.timestamp_epoch;

    return MeterReading{
        pulse.sequence,
        pulse.timestamp_epoch,
        total_pulses_,
        pulse.energy_wh,
        cumulative_energy_wh_ / 1000.0,
        power
    };
}

std::uint64_t SmartMeter::total_pulses() const {
    return total_pulses_;
}

double SmartMeter::cumulative_energy_kwh() const {
    return cumulative_energy_wh_ / 1000.0;
}
