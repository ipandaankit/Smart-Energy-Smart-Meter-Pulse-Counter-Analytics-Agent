#pragma once

#include <cstddef>
#include <string>

struct Config {
    std::string meter_id = "SM-001";
    double pulse_energy_wh = 1.0;
    double normal_min_watts = 20.0;
    double normal_max_watts = 5000.0;
    double anomaly_spike_watts = 7000.0;
    double anomaly_drop_watts = 5.0;
    std::size_t samples = 120;
    unsigned seed = 42;
    unsigned interval_seconds = 1;
    std::string telemetry_file = "data/telemetry.csv";
    std::string event_file = "data/events.log";
    std::string summary_file = "data/summary.json";
};

bool load_config(const std::string& path, Config& config, std::string& error);
