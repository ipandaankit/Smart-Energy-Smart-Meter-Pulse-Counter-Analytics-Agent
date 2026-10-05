#include "analyzer.hpp"

#include <algorithm>
#include <cmath>

namespace {
std::string event_name(EventType type) {
    switch (type) {
        case EventType::HighConsumption: return "HIGH_CONSUMPTION";
        case EventType::LowConsumption: return "LOW_CONSUMPTION";
        case EventType::PulseGap: return "PULSE_GAP";
        default: return "NORMAL";
    }
}
}

Analyzer::Analyzer(double min_watts, double max_watts,
                   double spike_watts, double drop_watts)
    : min_watts_(min_watts),
      max_watts_(max_watts),
      spike_watts_(spike_watts),
      drop_watts_(drop_watts) {}

AnalysisEvent Analyzer::analyze(const MeterReading& reading) {
    AnalysisEvent e;
    e.sequence = reading.sequence;
    e.timestamp_epoch = reading.timestamp_epoch;
    e.power_w = reading.estimated_power_w;

    if (reading.estimated_power_w >= spike_watts_ ||
        reading.estimated_power_w > max_watts_) {
        e.type = EventType::HighConsumption;
        e.message = event_name(e.type) + ": power=" +
                    std::to_string(reading.estimated_power_w) + " W";
    } else if (reading.estimated_power_w > 0.0 &&
               (reading.estimated_power_w <= drop_watts_ ||
                reading.estimated_power_w < min_watts_)) {
        e.type = EventType::LowConsumption;
        e.message = event_name(e.type) + ": power=" +
                    std::to_string(reading.estimated_power_w) + " W";
    } else if (reading.estimated_power_w == 0.0 && reading.sequence > 1) {
        e.type = EventType::PulseGap;
        e.message = event_name(e.type) + ": no valid interval power";
    } else {
        e.type = EventType::Normal;
        e.message = "NORMAL";
    }

    return e;
}

AnalyticsSummary Analyzer::summarize(
    const std::vector<MeterReading>& readings,
    const std::vector<AnalysisEvent>& events) const {

    AnalyticsSummary s;
    s.samples = readings.size();

    double power_sum = 0.0;
    for (const auto& r : readings) {
        s.total_energy_kwh = std::max(s.total_energy_kwh, r.cumulative_energy_kwh);
        s.peak_power_w = std::max(s.peak_power_w, r.estimated_power_w);
        if (r.estimated_power_w > 0.0) {
            power_sum += r.estimated_power_w;
        }
    }

    std::size_t powered_samples = 0;
    for (const auto& r : readings) {
        if (r.estimated_power_w > 0.0) ++powered_samples;
    }
    if (powered_samples) {
        s.average_power_w = power_sum / static_cast<double>(powered_samples);
    }

    for (const auto& e : events) {
        if (e.type == EventType::HighConsumption) ++s.high_events;
        else if (e.type == EventType::LowConsumption) ++s.low_events;
        else if (e.type == EventType::PulseGap) ++s.gap_events;
    }
    return s;
}
