#pragma once

#include "meter.hpp"
#include <string>
#include <vector>

enum class EventType {
    Normal,
    HighConsumption,
    LowConsumption,
    PulseGap
};

struct AnalysisEvent {
    std::uint64_t sequence{};
    std::uint64_t timestamp_epoch{};
    EventType type{EventType::Normal};
    double power_w{};
    std::string message;
};

struct AnalyticsSummary {
    std::size_t samples{};
    std::size_t high_events{};
    std::size_t low_events{};
    std::size_t gap_events{};
    double total_energy_kwh{};
    double average_power_w{};
    double peak_power_w{};
};

class Analyzer {
public:
    Analyzer(double min_watts, double max_watts, double spike_watts,
             double drop_watts);

    AnalysisEvent analyze(const MeterReading& reading);
    AnalyticsSummary summarize(const std::vector<MeterReading>& readings,
                               const std::vector<AnalysisEvent>& events) const;

private:
    double min_watts_;
    double max_watts_;
    double spike_watts_;
    double drop_watts_;
};
